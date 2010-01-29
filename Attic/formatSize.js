/**
 * Formats a size.
 *
 * @param {Number} bytes size in bytes
 * @returns size converted to a byte multiple unit
 * @type String
 */
function formatSize(bytes) {
    var units = ['B', 'KiB', 'MiB', 'GiB', 'TiB', 'PiB', 'EiB', 'ZiB', 'YiB'];
    
    for (var unit = 0; bytes >= 1024; ++unit) {
        bytes = Math.floor(bytes / 1024);
    }
    
    return bytes + ' ' + units[unit];
}
