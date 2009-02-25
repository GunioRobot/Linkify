var _plugin;


function addAccount(popup) {
    window.open('Add Account.html', 'addAccount');
}


function initialize() {
    _plugin = document.getElementById('Server');
    
    if (!('version' in _plugin)) {
        return;
    }
}


// Don't use the DOMContentLoaded event, or the plug-in won't be detected.
window.addEventListener('load', initialize, false);
