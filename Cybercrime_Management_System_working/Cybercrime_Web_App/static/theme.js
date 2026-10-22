(() => {
    const storageKey = 'cybercrime-management-theme';
    const root = document.documentElement;

    function setTheme(theme) {
        const normalized = theme === 'dark' ? 'dark' : 'light';
        root.dataset.theme = normalized;
        document.querySelectorAll('[data-theme-toggle]').forEach(button => {
            button.textContent = normalized === 'dark' ? 'Light mode' : 'Dark mode';
            button.setAttribute('aria-label', `Switch to ${normalized === 'dark' ? 'light' : 'dark'} mode`);
            button.setAttribute('aria-pressed', String(normalized === 'dark'));
        });
    }

    let initialTheme = 'light';
    try { initialTheme = localStorage.getItem(storageKey) || initialTheme; } catch (_) {}
    setTheme(initialTheme);

    document.querySelectorAll('[data-theme-toggle]').forEach(button => {
        button.addEventListener('click', () => {
            const nextTheme = root.dataset.theme === 'dark' ? 'light' : 'dark';
            setTheme(nextTheme);
            try { localStorage.setItem(storageKey, nextTheme); } catch (_) {}
        });
    });
})();
