#!/usr/bin/python
# -*- coding: utf8 -*-


# Standard library:
import ctypes, email


class Database (object):
    """
    Windows Mail database interface.
    """
    
    
    FOLDER_NAME_MAX_SIZE = None
    ROOT_FOLDER_ID = None
    
    
    def __init__(self):
        self.__dll = None    # Indicates the DLL wasn't loaded.
        self.__dll = ctypes.oledll.WindowsMail
        
        self.__dll.Initialize()
        
        self.FOLDER_NAME_MAX_SIZE = ctypes.c_size_t.in_dll(
            self.__dll, 'FOLDER_NAME_MAX_SIZE').value
        
        self.ROOT_FOLDER_ID = ctypes.c_size_t.in_dll(
            self.__dll, 'ROOT_FOLDER_ID').value
    
    
    def __del__(self):
        if self.__dll is not None:
            self.__dll.Finalize()
    
    
    def get_folder_name(self, folder):
        """
        Gets the name of a folder.
        
        @type folder: long
        @param folder: folder identifier
        @rtype: str
        @return: folder name
        """
        
        buffer = ctypes.create_string_buffer(self.FOLDER_NAME_MAX_SIZE)
        self.__dll.GetFolderName(folder, ctypes.byref(buffer))
        
        return buffer.value
    
    
    def get_message_data(self, folder, message):
        """
        Gets the raw contents of a message.
        
        @type folder: long
        @param folder: folder identifier
        @type message: long
        @param message: message identifier
        @rtype: bytearray
        @return: message contents
        """
        
        # Should a temporary file be used instead, in case of large messages?
        data = bytearray()
        
        Callback = ctypes.CFUNCTYPE(None, ctypes.c_char_p)
        self.__dll.GetMessageData(folder, message, Callback(data.extend))
        
        return data
    
    
    def get_messages(self, parent = ROOT_FOLDER_ID, path = []):
        """
        Creates a generator to iterate over all messages.
        
        @type parent: long
        @param parent: identifier of the starting point folder
        @type path: list
        @param path: folder names path
        @rtype: tuple
        @return: yields the message path and the message itself
        """
        
        for folder in self.list_folders(parent):
            name = self.get_folder_name(folder)
            
            for message in self.list_messages(folder):
                data = self.get_message_data(folder, message)
                
                yield (email.message_from_string(data), path + [name])
            
            for message, path in self.get_messages(folder, path + [name]):
                yield (message, path)
    
    
    def list_folders(self, parent = ROOT_FOLDER_ID):
        """
        Lists folders.
        
        @type parent: long
        @param parent: identifier of the parent folder
        @rtype: list
        @return: identifiers of the child folders
        """
        
        folders = []
        
        Callback = ctypes.CFUNCTYPE(None, ctypes.c_size_t)
        self.__dll.ListFolders(parent, Callback(folders.append))
        
        return folders
    
    
    def list_messages(self, folder):
        """
        Lists messages.
        
        @type folder: long
        @param folder: identifier of the containing folder
        @rtype: list
        @return: identifiers of all messages found
        """
        
        messages = []
        
        Callback = ctypes.CFUNCTYPE(None, ctypes.c_size_t)
        self.__dll.ListMessages(folder, Callback(messages.append))
        
        return messages


if __name__ == '__main__':
    print 'Messages:'
    
    for message, path in Database().get_messages():
        print '- %Xh in %s' % (hash(message), path)
