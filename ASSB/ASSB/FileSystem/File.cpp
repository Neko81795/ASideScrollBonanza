/*!***************************************************************************
@file    File.cpp
@author  Reverie Wisp
@par     Email: contact@rw0.pw
@date    11/19/2016

@brief
Lifted from my ASCIIPlayer project.
*****************************************************************************/
#include "File.hpp"



namespace FileSystem
{
  // Constructor for FileProxy
  FileProxy::FileProxy(unsigned int line, File &f) 
    : f_(f)
    , line_(line)
  {  }


	FileProxy::FileProxy(const FileProxy &rhs)
		: f_(rhs.f_)
		, line_(rhs.line_)
	{  }

  // Actually writes line(s) in question.
  FileProxy &FileProxy::operator =(std::string str)
  {
    f_.Write(str, static_cast<int>(line_));
    return *this;
  }


  // Attempt to access the line in question.
  // Will throw exception if line does not exist.
  FileProxy::operator std::string() const
  {
    return f_.GetLine(line_);
  }


  // Printing the FileProxy
  std::ostream& operator<<(std::ostream &os, const FileProxy &f)
  {
    os << f.f_.GetLine(f.line_);
    return os;
  }


  // Constructor.
  // Attempts to open the file in question, if no file was found,
  // automatically set to output.
  File::File(std::string filepath, int mode)
    : filepath_(filepath)
    , currentFileType_((mode & Binary) ? Binary : Text)
    , currentModifyMode_((mode & Join) ? Join : Replace) 
    , fileName_("")
    , fileExtension_("")
    , fileLocation_("")
    , lastModifiedLine_(0)
    , foundFile_(false)
  {
    // Reads the current file.
    foundFile_ = readCurrent();

    // Update information if possible.
    fileLocation_ = parseFileLocation();
    fileName_ = parseFileName();
    fileExtension_ = parseExtension();
  }


  // Destructor - Cleanup
  File::~File()
  {  }


  // Gets the specified line. No bounds checking performed.
  FileProxy File::operator[](unsigned int line)
  {
    return FileProxy(line, *this);
  }

  std::string File::GetLine(unsigned int line) const
  {
    return fileLines_[line];
  }

  // Gets the specified line. No bounds checking performed.
  std::string File::operator[](unsigned int line) const
  {
    return fileLines_[line];
  }


  // Writes arbitrary data to the file. Will disregard line if used in binary mode.
  void File::Write(const void *data, size_t dataSizeBytes, int line)
  {
    if (currentFileType_ == Binary)
    {
      // Resize if necessary
      if (fileLines_.size() < 1)
        fileLines_.resize(1);

      // Append if necessary.
      if (currentModifyMode_ == Join)
        fileLines_[0] = fileLines_[0] + std::string(reinterpret_cast<const char *>(data), dataSizeBytes);
      else // Current mode is Replace
        fileLines_[0] = std::string(reinterpret_cast<const char *>(data), dataSizeBytes);
    }
    else
    {
      if (line < 0)
        line = static_cast<int>(lastModifiedLine_);

      // Resize to accomodate for adding to the end.
      if (static_cast<size_t>(line) >= fileLines_.size())
        fileLines_.resize(static_cast<size_t>(line + 1));

      // Append if necessary.
      if (currentModifyMode_ == Join)
        fileLines_[static_cast<size_t>(line)] 
				= fileLines_[static_cast<size_t>(line)] + std::string(reinterpret_cast<const char *>(data), dataSizeBytes);
      else // Current mode is Replace
        fileLines_[0] = std::string(reinterpret_cast<const char *>(data), dataSizeBytes);
    }
  }


  // Write to a specific line in the file.
  // Note that line and newlineDelimiter are completely disregarded for binary files.
  void File::Write(std::string toWrite, int line, const std::string newlineDelimiter)
  {
    // Handle binary case
    if (currentFileType_ == Binary)
    {
      // Resize if necessary
      if(fileLines_.size() < 1)
        fileLines_.resize(1);

      // Append if necessary.
      if (currentModifyMode_ == Join)
        fileLines_[0] = fileLines_[0] + toWrite;
      else // Current mode is Replace
        fileLines_[0] = toWrite;
    }
    else
    {
      // Prep location for new line.


      // Holds split lines.
      std::vector<std::string> splitLines;

      // Parse out at newline character.
      size_t pos;
      while ((pos = toWrite.find(newlineDelimiter)) != toWrite.npos)
      {
        splitLines.push_back(toWrite.substr(0, pos));
        toWrite.erase(0, pos + newlineDelimiter.size());
      }
      splitLines.push_back(toWrite);

      // Update line if necessary.
      if (line < 0)
        line = static_cast<int>(lastModifiedLine_);

      // Resize to accomodate for adding to the end.
      unsigned int newSize = static_cast<unsigned int>(splitLines.size() + line);
      if (fileLines_.size() < newSize)
        fileLines_.resize(newSize);

      // Now that we have all the lines split, we either join toor write over  existing lines.
      int lineWithOffset = line;
      for (std::size_t i = 0; i < splitLines.size(); ++i)
      {
        if (currentModifyMode_ == Join)
          fileLines_[static_cast<size_t>(lineWithOffset)] = fileLines_[static_cast<size_t>(line + i)] + splitLines[i];
        else // Current mode is Replace
          fileLines_[static_cast<size_t>(lineWithOffset)] = splitLines[i];
        lineWithOffset++;
      }

      // Update last modified.
      lastModifiedLine_ = static_cast<unsigned int>(splitLines.size() - 1);
    }
  }


  // Writes file to output stream.
  std::ostream& operator<<(std::ostream &os, File &f)
  {
    for (size_t i = 0; i < f.fileLines_.size(); ++i)
      os << f.fileLines_[i] << std::endl;

    return os;
  }


  // Write the file changes
  bool File::writeChangesToFile(bool writeOver)
  {
    // Check to see if we will be writing over.
    std::fstream fTest(filepath_, std::ios::in);
    if (fTest.is_open())
    {
      //If we are not overriding, leave. Close then.
      if (!writeOver)
      {
        fTest.close();
        return false;
      }
    }

    // Establish the file mode flags
    std::ios::openmode mode = std::ios::out;
    mode |= std::ios::trunc;
    if (currentFileType_ == Binary)
      mode |= std::ios::binary;

    // Create file object
    std::fstream fileObject(fileLocation_ + fileName_, mode);

    // See if we opened the file successfully.
    if (fileObject.is_open())
    {
      if (currentFileType_ == Binary)
      {
        for (std::size_t i = 0; i < fileLines_.size(); ++i)
          fileObject << fileLines_[i];
      }
      else
      {
        for (std::size_t i = 0; i < fileLines_.size(); ++i)
          fileObject << fileLines_[i] << std::endl;
      }
      fileObject.close();

      // We were successful
      return true;
    }

    // Return write error
    return false;
  }


  // ReReads the file into memory from the current filepath.
  // WARNING: ERASES ALL LOCAL MODIFICATIONS AND OVERRIDES CHANGES IF UNWRITTEN
  bool File::ReRead()
  {
    if (readCurrent())
    {
      // Update information if possible.
      fileLocation_ = parseFileLocation();
      fileName_ = parseFileName();
      fileExtension_ = parseExtension();

      // We read and updated stuff!
      return true;
    }

    // Could not re-read, did not update info.
    return false;
  }


  // Reads from scratch the file in question, resetting the object.
  bool File::readCurrent()
  {
    // Check read style
    std::ios::openmode mode = std::ios::in;
    if (currentFileType_ == Binary)
      mode |= std::ios::binary;

    // Create file object
    std::fstream fileObject(filepath_, mode);

    // See if we opened the file successfully.
    if (fileObject.is_open())
    {
      // See if we can read the file
      std::string buffer;
      while (std::getline(fileObject, buffer))
        fileLines_.push_back(buffer);
      fileObject.close();

      // We read the file in successfully
      return true;
    }

    // Could not open file for whatever reason.
    return false;
  }


  // Getters
  bool         File::FileFound() const      { return foundFile_;        }
  std::string File::GetFileLocation() const { return fileLocation_;     }
  std::string File::GetFileName() const     { return fileName_;         }
  std::string File::GetExtension() const    { return fileExtension_;    }
	unsigned int File::GetLineCount() const   { return static_cast<unsigned int>(fileLines_.size()); }


  // Clears contents of the read file, if it exists.
  void File::Clear()
  {
    fileLines_.clear();
  }


  // Saves changes to file.
  bool File::Save()
  {
    return writeChangesToFile(true);
  }


  // Appends one file to another.
  void File::Append(const File &rhs)
  {
    fileLines_.insert(fileLines_.end(), rhs.fileLines_.begin(), rhs.fileLines_.end());
  }


  // Appends another file to the end of this one.
  void File::operator+=(const File &rhs)
  {
    Append(rhs);
  }


    ///////////////////////////////////////
   // General Info and associated funcs //
  ///////////////////////////////////////
  // Save file with a different filepath and name, with bool to override or not.
  bool File::SaveAs(std::string newPath, bool override)
  {
    // Get new filepath.
    filepath_ = newPath;

    // Update information, if possible.
    fileLocation_ = parseFileLocation();
    fileName_ = parseFileName();
    fileExtension_ = parseExtension();

    // Write changes.
    return writeChangesToFile(override);
  }


  // Parses the file location, or folder, the file is in.
  std::string File::parseFileLocation()
  {
    std::string path = "";
    std::size_t location = filepath_.find_last_of("/\\");
    if (location != filepath_.npos)
      path = filepath_.substr(0, location + 1);
    return path;
  }


  // Gets the name of the file with path removed.
  std::string File::parseFileName()
  {
    std::string name = filepath_;
    std::size_t location = filepath_.find_last_of("/\\");
    if(location != filepath_.npos)
      name = filepath_.substr(location + 1);
    return name;
  }


  // Gets the extension of the file, if applicable. Otherwise, returns empty string.
  std::string File::parseExtension()
  {
    std::string extension = "";
    std::size_t location = filepath_.find_last_of(".");
    if (location != filepath_.npos)
      extension = filepath_.substr(location + 1);
    return extension;
  }


  // Gets the contents of the file as 
  std::vector<std::string> File::GetContents() const
  {
    return fileLines_;
  }
}

