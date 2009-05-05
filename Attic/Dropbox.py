#!/usr/bin/python
# -*- coding: utf-8 -*-


# Standard library:
import base64, os, pickle, pprint, sqlite3, struct, subprocess, sys, time, \
       _winreg

# Python Extensions for Windows:
import pywintypes, win32api, win32con, win32gui, win32pdh, win32pipe, win32ts


__version__ = '2009-05-05'


class Dropbox (object):
    STATUS_IDLE = 'Idle.'
    STATUS_NOT_CONNECTED = 'Not connected.'
    STATUS_NOT_MONITORING = 'Not monitoring.'
    STATUS_NOT_RUNNING = 'Not running.'
    STATUS_SYNCHRONIZING = 'Synchronizing...'
    
    
    @property
    def config_table(self):
        """
        Configuration database table.
        
        @author: U{Steve H.<http://wiki.getdropbox.com/DropboxAddons/PythonScriptToDisplayConfig>}
        """
        
        configuration = {}
        
        for (index, key, value) in self.read_database_table('config'):
            if value is not None:
                configuration[key] = pickle.loads(base64.b64decode(value))
        
        return configuration
    
    
    @property
    def database_path(self):
        """
        Path to the database file.
        
        @raise Exception: if not found
        """
        
        file = self.name.lower() + '.db'
        paths = [
            ['%APPDATA%', self.name, file],
            ['$HOME', '.' + self.name.lower(), file],
        ]
        
        for path in [os.path.expandvars(os.path.join(*p)) for p in paths]:
            if os.path.exists(path):
                return path
        
        raise Exception('Database not found.')
    
    
    @property
    def database_tables(self):
        """
        List of database tables.
        """
        
        database = sqlite3.connect(self.database_path)
        cursor = database.cursor()
        cursor.execute('select name from sqlite_master where type = "table"')
        
        tables = [name for (name,) in cursor]
        database.close()
        
        return tables
    
    
    @property
    def installation_path(self):
        """
        Path to the installation directory.
        
        @raise Exception: if not found
        """
        
        branch = 'SOFTWARE\\Evenflow Software\\' + self.name
        root_key = _winreg.OpenKey(_winreg.HKEY_LOCAL_MACHINE, branch)
        i = 0
        
        try:
            while True:
                (key, value, type) = _winreg.EnumValue(root_key, i)
                i += 1
                
                if key == 'InstallPath':
                    return value
        except WindowsError:
            raise Exception('Installation not found.')
    
    
    @property
    def name(self):
        """
        Application's name.
        """
        
        return self.__class__.__name__
    
    
    @property
    def path(self):
        """
        Path to the root of the directory to be synchronized.
        """
        
        return self.config_table[self.name.lower() + '_path']
    
    
    @property
    def process_id(self):
        """
        Application's process identifier.
        
        @raise Exception: if not running
        """
        
        detail = win32pdh.PERF_DETAIL_WIZARD
        (items, objs) = win32pdh.EnumObjectItems(None, None, 'Process', detail)
        
        if self.name not in objs:
            raise Exception('Not running.')
        
        query = win32pdh.OpenQuery()
        path_elements = (None, 'Process', self.name, None, 0, 'ID Process')
        path = win32pdh.MakeCounterPath(path_elements)
        counter = win32pdh.AddCounter(query, path)
        Long = win32pdh.PDH_FMT_LONG
        
        win32pdh.CollectQueryData(query)
        (type, id) = win32pdh.GetFormattedCounterValue(counter, Long)
        win32pdh.RemoveCounter(counter)
        win32pdh.CloseQuery(query)
        
        return id
    
    
    @property
    def status(self):
        """
        Application status code.
        
        @author: U{Steve H.<http://wiki.getdropbox.com/DropboxAddons/PythonScriptToGetFileOrFolderStatusInWindows>}
        """
        
        proc_id = win32api.GetCurrentProcessId()
        thread_id = win32api.GetCurrentThreadId()
        session_id = win32ts.ProcessIdToSessionId(proc_id)
        
        req_type = 1
        req_data = struct.pack('LLLL', 0x3048302, proc_id, thread_id, req_type)
        
        pipe_name = r'\\.\PIPE\%sPipe_%d' % (self.name, session_id)
        request = (req_data + self.path.encode('utf-16') + (chr(0) * 540))[0:540]
        buffer_size = 16382
        
        try:
            response = win32pipe.CallNamedPipe(pipe_name, request, buffer_size,
                win32pipe.NMPWAIT_USE_DEFAULT_WAIT)
        except pywintypes.error as error:
            if error[0] != 2:
                raise
            
            try:
                self.process_id
                return self.STATUS_NOT_CONNECTED
            except:
                return self.STATUS_NOT_RUNNING
        
        status_codes = {
            0: self.STATUS_NOT_MONITORING,
            1: self.STATUS_IDLE,
            2: self.STATUS_SYNCHRONIZING,
            3: self.STATUS_NOT_CONNECTED,
        }
        
        return status_codes[int(response[4:-1])]
    
    
    def read_database_table(self, name):
        """
        Reads a table from the database.
        
        @type name: basestring
        @param name: name of the table to iterate
        @rtype: tuple
        @return: yields each table row
        """
        
        database = sqlite3.connect(self.database_path)
        cursor = database.cursor()
        cursor.execute('select * from ' + name)
        
        for row in cursor:
            yield row
        
        database.close()
    
    
    def start(self):
        """
        Starts the application.
        """
        
        if self.status == self.STATUS_NOT_RUNNING:
            executable = os.path.join(self.installation_path, self.name + '.exe')
            subprocess.Popen(executable)
    
    
    def stop(self, force = False):
        """
        Stops the application.
        
        @type force: bool
        @param force: whether or not to force termination without waiting
        """
        
        if not force:
            while self.status == self.STATUS_SYNCHRONIZING:
                time.sleep(1)
        
        if self.status == self.STATUS_NOT_RUNNING:
            return
        
        id = self.process_id
        handle = win32api.OpenProcess(win32con.PROCESS_TERMINATE, False, id)
        
        try:
            win32api.TerminateProcess(handle, 0)
            time.sleep(1)
        finally:
            win32api.CloseHandle(handle)


if __name__ == '__main__':
    args = sys.argv[1:]
    
    if len(args) != 1:
        sys.exit('Usage: <command or property>')
    
    dropbox = Dropbox()
    
    try:
        command = getattr(dropbox, args[0])
    except AttributeError:
        sys.exit('Error: Invalid command or property.')
    
    if callable(command):
        command()
    else:
        printer = pprint.PrettyPrinter()
        printer.pprint(command)
