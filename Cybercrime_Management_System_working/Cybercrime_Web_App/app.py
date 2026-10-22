from flask import Flask, render_template, request, jsonify, send_from_directory, abort
import mysql.connector
import os
from pathlib import Path
from uuid import uuid4
from werkzeug.utils import secure_filename

app = Flask(__name__)
app.config['MAX_CONTENT_LENGTH'] = 25 * 1024 * 1024
EVIDENCE_UPLOAD_DIR = Path(app.instance_path) / 'evidence_uploads'
INCIDENT_STATUSES = {'Open', 'Under Investigation', 'Closed', 'Resolved'}

DB_CONFIG = {
    'host': 'localhost',
    'user': 'root',
    'password': os.environ.get('DB_PASSWORD', ''),
    'database': 'cybercrime_db'
}

def get_db_connection():
    return mysql.connector.connect(**DB_CONFIG)

@app.route('/')
def index():
    return render_template('home.html')

@app.route('/incidents')
def incidents_page():
    return render_template('index.html')

@app.route('/evidence')
def evidence_page():
    return render_template('evidence.html')

def ensure_evidence_files_table(cursor):
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS evidence_files (
            file_id INT AUTO_INCREMENT PRIMARY KEY,
            evidence_id VARCHAR(50) NOT NULL,
            original_filename VARCHAR(255) NOT NULL,
            stored_filename VARCHAR(100) NOT NULL UNIQUE,
            uploaded_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (evidence_id) REFERENCES evidence(evidence_id) ON DELETE CASCADE
        )
    """)

@app.route('/api/incidents', methods=['GET'])
def get_incidents():
    try:
        conn = get_db_connection()
        cursor = conn.cursor(dictionary=True)
        cursor.execute("""
            SELECT case_id, type AS incident_type, severity, status, description,
                   extra_data AS target_or_hash, lead_investigator_id
            FROM incidents
            ORDER BY created_at DESC
        """)
        incidents = cursor.fetchall()
        cursor.close()
        conn.close()
        return jsonify(incidents)
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/api/incidents', methods=['POST'])
def add_incident():
    data = request.get_json(silent=True) or {}
    status = str(data.get('status', 'Open')).strip()
    if status not in INCIDENT_STATUSES:
        return jsonify({'error': 'Choose a valid case status.'}), 400
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        
        query = """
        INSERT INTO incidents (case_id, type, severity, status, description, extra_data, lead_investigator_id)
        VALUES (%s, %s, %s, %s, %s, %s, %s)
        """
        values = (
            data['case_id'],
            data['incident_type'],
            data['severity'],
            status,
            data['description'],
            data['target_or_hash'],
            data['lead_investigator_id']
        )
        
        cursor.execute(query, values)
        conn.commit()
        cursor.close()
        conn.close()
        return jsonify({'message': 'Case created successfully!'}), 201
    except Exception as e:
        return jsonify({'error': str(e)}), 400

@app.route('/api/incidents/<string:case_id>/close', methods=['PATCH'])
def close_incident(case_id):
    conn = None
    cursor = None
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute("UPDATE incidents SET status = 'Closed' WHERE case_id = %s", (case_id,))
        if cursor.rowcount == 0:
            cursor.execute('SELECT case_id FROM incidents WHERE case_id = %s', (case_id,))
            if not cursor.fetchone():
                conn.rollback()
                return jsonify({'error': 'Incident not found.'}), 404
        conn.commit()
        return jsonify({'message': f'Case {case_id} is now closed.'})
    except Exception as e:
        if conn:
            conn.rollback()
        return jsonify({'error': str(e)}), 400
    finally:
        if cursor:
            cursor.close()
        if conn:
            conn.close()

@app.route('/api/incidents/<string:case_id>', methods=['PATCH'])
def edit_incident(case_id):
    data = request.get_json(silent=True) or {}
    fields = ('incident_type', 'severity', 'status', 'description', 'target_or_hash')
    if any(not str(data.get(field, '')).strip() for field in fields):
        return jsonify({'error': 'Incident type, severity, status, description, and target/hash are required.'}), 400
    if str(data['status']).strip() not in INCIDENT_STATUSES:
        return jsonify({'error': 'Choose a valid case status.'}), 400

    conn = None
    cursor = None
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute(
            """UPDATE incidents
               SET type = %s, severity = %s, status = %s, description = %s,
                   extra_data = %s, lead_investigator_id = %s
               WHERE case_id = %s""",
            tuple(str(data[field]).strip() for field in fields)
            + (str(data.get('lead_investigator_id', '')).strip() or None, case_id)
        )
        if cursor.rowcount == 0:
            cursor.execute('SELECT case_id FROM incidents WHERE case_id = %s', (case_id,))
            if not cursor.fetchone():
                conn.rollback()
                return jsonify({'error': 'Incident not found.'}), 404
        conn.commit()
        return jsonify({'message': f'Case {case_id} updated successfully.'})
    except Exception as e:
        if conn:
            conn.rollback()
        return jsonify({'error': str(e)}), 400
    finally:
        if cursor:
            cursor.close()
        if conn:
            conn.close()

@app.route('/api/evidence', methods=['GET'])
def get_evidence():
    conn = None
    cursor = None
    try:
        conn = get_db_connection()
        cursor = conn.cursor(dictionary=True)
        cursor.execute("""
            SELECT evidence.evidence_id, evidence.case_id,
                   incidents.type AS incident_type, evidence.type AS evidence_type,
                   evidence.custody_log, evidence.created_at
            FROM evidence
            INNER JOIN incidents ON incidents.case_id = evidence.case_id
            ORDER BY evidence.created_at DESC
        """)
        return jsonify(cursor.fetchall())
    except Exception as e:
        return jsonify({'error': str(e)}), 500
    finally:
        if cursor:
            cursor.close()
        if conn:
            conn.close()

@app.route('/api/evidence', methods=['POST'])
def add_evidence():
    data = request.get_json(silent=True) or {}
    required_fields = ('evidence_id', 'case_id', 'evidence_type', 'custody_log')
    missing_fields = [field for field in required_fields if not str(data.get(field, '')).strip()]
    if missing_fields:
        return jsonify({'error': 'Please provide an evidence ID, incident, evidence type, and custody log.'}), 400

    conn = None
    cursor = None
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute(
            """INSERT INTO evidence (evidence_id, case_id, type, custody_log)
               VALUES (%s, %s, %s, %s)""",
            (
                str(data['evidence_id']).strip(),
                str(data['case_id']).strip(),
                str(data['evidence_type']).strip(),
                str(data['custody_log']).strip()
            )
        )
        conn.commit()
        return jsonify({'message': 'Evidence linked to incident successfully.'}), 201
    except Exception as e:
        if conn:
            conn.rollback()
        return jsonify({'error': str(e)}), 400
    finally:
        if cursor:
            cursor.close()
        if conn:
            conn.close()

@app.route('/investigators')
def investigators_page():
    return render_template('investigators.html')

@app.route('/api/investigators', methods=['GET'])
def get_investigators():
    conn = None
    cursor = None
    try:
        conn = get_db_connection()
        cursor = conn.cursor(dictionary=True)
        cursor.execute("""
            SELECT investigator_id, name, contact, created_at
            FROM investigators
            ORDER BY name ASC
        """)
        return jsonify(cursor.fetchall())
    except Exception as e:
        return jsonify({'error': str(e)}), 500
    finally:
        if cursor:
            cursor.close()
        if conn:
            conn.close()

@app.route('/api/investigators', methods=['POST'])
def add_investigator():
    data = request.get_json(silent=True) or {}
    fields = ('investigator_id', 'name', 'contact')
    if any(not str(data.get(field, '')).strip() for field in fields):
        return jsonify({'error': 'Investigator ID, name, and contact are required.'}), 400
    conn = None
    cursor = None
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute(
            'INSERT INTO investigators (investigator_id, name, contact) VALUES (%s, %s, %s)',
            tuple(str(data[field]).strip() for field in fields)
        )
        conn.commit()
        return jsonify({'message': 'Investigator added successfully.'}), 201
    except Exception as e:
        if conn:
            conn.rollback()
        return jsonify({'error': str(e)}), 400
    finally:
        if cursor:
            cursor.close()
        if conn:
            conn.close()

@app.route('/reports')
def reports_page():
    return render_template('reports.html')

@app.route('/api/reports', methods=['GET'])
def get_reports():
    conn = None
    cursor = None
    try:
        conn = get_db_connection()
        cursor = conn.cursor(dictionary=True)
        cursor.execute("""
            SELECT reports.id, reports.case_id, incidents.type AS incident_type,
                   reports.report_text, reports.created_at
            FROM reports
            INNER JOIN incidents ON incidents.case_id = reports.case_id
            ORDER BY reports.created_at DESC
        """)
        return jsonify(cursor.fetchall())
    except Exception as e:
        return jsonify({'error': str(e)}), 500
    finally:
        if cursor:
            cursor.close()
        if conn:
            conn.close()

@app.route('/api/reports', methods=['POST'])
def add_report():
    data = request.get_json(silent=True) or {}
    case_id = str(data.get('case_id', '')).strip()
    report_text = str(data.get('report_text', '')).strip()
    if not case_id or not report_text:
        return jsonify({'error': 'Choose an incident and enter report details.'}), 400
    conn = None
    cursor = None
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute(
            'INSERT INTO reports (case_id, report_text) VALUES (%s, %s)',
            (case_id, report_text)
        )
        conn.commit()
        return jsonify({'message': 'Report saved successfully.'}), 201
    except Exception as e:
        if conn:
            conn.rollback()
        return jsonify({'error': str(e)}), 400
    finally:
        if cursor:
            cursor.close()
        if conn:
            conn.close()

@app.route('/api/reports/<int:report_id>', methods=['DELETE'])
def delete_report(report_id):
    conn = None
    cursor = None
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute('DELETE FROM reports WHERE id = %s', (report_id,))
        if cursor.rowcount == 0:
            conn.rollback()
            return jsonify({'error': 'Report not found.'}), 404
        conn.commit()
        return jsonify({'message': 'Report deleted successfully.'})
    except Exception as e:
        if conn:
            conn.rollback()
        return jsonify({'error': str(e)}), 400
    finally:
        if cursor:
            cursor.close()
        if conn:
            conn.close()

@app.route('/api/evidence-files', methods=['GET'])
def get_evidence_files():
    conn = None
    cursor = None
    try:
        conn = get_db_connection()
        cursor = conn.cursor(dictionary=True)
        ensure_evidence_files_table(cursor)
        cursor.execute("""
            SELECT files.file_id, files.evidence_id, files.original_filename,
                   files.uploaded_at, evidence.case_id
            FROM evidence_files AS files
            INNER JOIN evidence ON evidence.evidence_id = files.evidence_id
            ORDER BY files.uploaded_at DESC
        """)
        return jsonify(cursor.fetchall())
    except Exception as e:
        return jsonify({'error': str(e)}), 500
    finally:
        if cursor:
            cursor.close()
        if conn:
            conn.close()

@app.route('/api/evidence-files', methods=['POST'])
def upload_evidence_file():
    evidence_id = request.form.get('evidence_id', '').strip()
    uploaded_files = [item for item in request.files.getlist('files') if item and item.filename]
    if not evidence_id or not uploaded_files:
        return jsonify({'error': 'Choose an evidence record and at least one file to upload.'}), 400

    files_to_save = []
    for uploaded_file in uploaded_files:
        original_filename = secure_filename(uploaded_file.filename)
        if not original_filename:
            return jsonify({'error': 'One of the selected files has an invalid filename.'}), 400
        files_to_save.append((uploaded_file, original_filename, f'{uuid4().hex}_{original_filename}'))

    conn = None
    cursor = None
    saved_paths = []
    try:
        EVIDENCE_UPLOAD_DIR.mkdir(parents=True, exist_ok=True)
        conn = get_db_connection()
        cursor = conn.cursor()
        ensure_evidence_files_table(cursor)
        for uploaded_file, original_filename, stored_filename in files_to_save:
            saved_path = EVIDENCE_UPLOAD_DIR / stored_filename
            uploaded_file.save(saved_path)
            saved_paths.append(saved_path)
            cursor.execute(
                """INSERT INTO evidence_files (evidence_id, original_filename, stored_filename)
                   VALUES (%s, %s, %s)""",
                (evidence_id, original_filename, stored_filename)
            )
        conn.commit()
        return jsonify({'message': f'{len(files_to_save)} file(s) attached to evidence successfully.'}), 201
    except Exception as e:
        if conn:
            conn.rollback()
        for saved_path in saved_paths:
            saved_path.unlink(missing_ok=True)
        return jsonify({'error': str(e)}), 400
    finally:
        if cursor:
            cursor.close()
        if conn:
            conn.close()

@app.route('/evidence-files/<int:file_id>/download')
def download_evidence_file(file_id):
    conn = None
    cursor = None
    try:
        conn = get_db_connection()
        cursor = conn.cursor(dictionary=True)
        ensure_evidence_files_table(cursor)
        cursor.execute(
            'SELECT original_filename, stored_filename FROM evidence_files WHERE file_id = %s',
            (file_id,)
        )
        file_record = cursor.fetchone()
        if not file_record:
            abort(404)
    except Exception:
        abort(404)
    finally:
        if cursor:
            cursor.close()
        if conn:
            conn.close()
    return send_from_directory(
        EVIDENCE_UPLOAD_DIR,
        file_record['stored_filename'],
        as_attachment=True,
        download_name=file_record['original_filename']
    )

@app.errorhandler(413)
def upload_too_large(_error):
    return jsonify({'error': 'Files must be 25 MB or smaller.'}), 413

if __name__ == '__main__':
    app.run(debug=True, port=5000)
