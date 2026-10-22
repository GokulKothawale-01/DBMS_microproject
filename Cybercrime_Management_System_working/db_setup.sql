CREATE DATABASE IF NOT EXISTS cybercrime_db;
USE cybercrime_db;

DROP TABLE IF EXISTS reports;
DROP TABLE IF EXISTS evidence_files;
DROP TABLE IF EXISTS evidence;
DROP TABLE IF EXISTS investigators;
DROP TABLE IF EXISTS incidents;

CREATE TABLE incidents (
    id INT AUTO_INCREMENT PRIMARY KEY,
    case_id VARCHAR(50) NOT NULL UNIQUE,
    type VARCHAR(30) NOT NULL,
    severity VARCHAR(20) NOT NULL,
    status VARCHAR(30) NOT NULL,
    description TEXT NOT NULL,
    extra_data TEXT,
    lead_investigator_id VARCHAR(50),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE evidence (
    id INT AUTO_INCREMENT PRIMARY KEY,
    evidence_id VARCHAR(50) NOT NULL UNIQUE,
    case_id VARCHAR(50) NOT NULL,
    type VARCHAR(50) NOT NULL,
    custody_log TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (case_id) REFERENCES incidents(case_id)
);

CREATE TABLE evidence_files (
    file_id INT AUTO_INCREMENT PRIMARY KEY,
    evidence_id VARCHAR(50) NOT NULL,
    original_filename VARCHAR(255) NOT NULL,
    stored_filename VARCHAR(100) NOT NULL UNIQUE,
    uploaded_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (evidence_id) REFERENCES evidence(evidence_id) ON DELETE CASCADE
);

CREATE TABLE investigators (
    id INT AUTO_INCREMENT PRIMARY KEY,
    investigator_id VARCHAR(50) NOT NULL UNIQUE,
    name VARCHAR(100) NOT NULL,
    contact VARCHAR(100) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE reports (
    id INT AUTO_INCREMENT PRIMARY KEY,
    case_id VARCHAR(50) NOT NULL,
    report_text TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (case_id) REFERENCES incidents(case_id)
);

INSERT INTO incidents (case_id, type, severity, status, description, extra_data)
VALUES
('CYB1001', 'Phishing', 'High', 'Under Investigation', 'Fake bank login page detected', 'https://phish.example.com/login'),
('CYB1002', 'Malware', 'Medium', 'Open', 'Suspicious ransomware behavior on workstation', 'abc123hash456');

INSERT INTO evidence (evidence_id, case_id, type, custody_log)
VALUES
('EV-001', 'CYB1001', 'Screenshot', 'Collected by officer -> Stored in evidence locker'),
('EV-002', 'CYB1002', 'Log File', 'Captured from endpoint -> Verified by analyst');

INSERT INTO investigators (investigator_id, name, contact)
VALUES
('INV-001', 'Aisha Khan', 'aisha@cyberunit.com'),
('INV-002', 'Rohan Mehta', 'rohan@cyberunit.com');
