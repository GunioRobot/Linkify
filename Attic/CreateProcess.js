var shell = new ActiveXObject('WScript.Shell');
var fileSystem = new ActiveXObject('Scripting.FileSystemObject');
var ForReading = 1;
var listFileName = fileSystem.GetBaseName(WScript.ScriptFullName) + '.txt';
var listFile = fileSystem.OpenTextFile(listFileName, ForReading, true);

while (!listFile.AtEndOfStream) {
    shell.Run(listFile.ReadLine());
}
