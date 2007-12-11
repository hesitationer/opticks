/*
 * The information in this file is
 * Copyright(c) 2007 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */



#ifndef XMLBASE_H__
#define XMLBASE_H__

#include <stdio.h>

#include "XercesIncludes.h"

#include "Filename.h"
#include "MessageLogMgr.h"
#include "MessageLog.h"

/** @file xmlbase.h
 *  @brief XML utilities and functionality common to reading and writing
 */
/** @defgroup app_xml Application XML system */

/**
 * This automatically handles convertions between Unicode and ASCII format.
 *
 * @ingroup app_xml
 *
 * @par requirements
 * Apache Xerces-C++ verion 2.4.0
 */
class OpticksXStr
{
   XMLCh *fUnicodeForm;
   char *fAsciiForm;
   int releaseWhich;
public:
   /**
    * Create an %OpticksXStr object containing an ASCII string.
    * @param toTranscode
    *        The ASCII string
    */
   OpticksXStr(const char *toTranscode)
      {
         fAsciiForm=const_cast<char *>(toTranscode);
         fUnicodeForm=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toTranscode);
         releaseWhich=1;
      }

   /**
    * Create an %OpticksXStr object containing a Unicode string.
    * @param unicode
    *        The Unicode string
    */
   OpticksXStr(const XMLCh *unicode)
      {
         fUnicodeForm = const_cast<XMLCh *>(unicode);
         fAsciiForm=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(unicode);
         releaseWhich=2;
      }

   /**
    * Destroy the %OpticksXStr, cleaning up Xerces allocated memory as needed.
    */
   ~OpticksXStr()
      {
         if(releaseWhich == 1)
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&fUnicodeForm);
         else if(releaseWhich = 2)
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&fAsciiForm);
      }

   /**
    * Obtain the Unicode version of this %OpticksXStr
    *
    * @return the Unicode form
    */
   const XMLCh *unicodeForm() const
      {
         return fUnicodeForm;
      }

   /**
    * Obtain the ASCII version of this %OpticksXStr
    *
    * @return the ASCII form
    */
   const char *asciiForm() const
      {
         return fAsciiForm;
      }
};

/**
 * Syntactic shortcut which converts the argument to ASCII
 *
 * @ingroup app_xml
 *
 * @param str
 *        The ASCII or Unicode string to convert to ASCII
 *
 * @return The ASCII representation of %str
 */
#define A(str) OpticksXStr(str).asciiForm()

/**
 * Common functionality for reading and writing XML
 * @ingroup app_xml
 *
 * @par requirements
 * Apache Xerces-C++ verion 2.4.0
 */
class XmlBase
{
public: // static helpers
   /**
    * This function will convert a \b file:// URL to a path.
    *
    * It handles Windows drive letters reasonably;
    * however, you will not be able to refer to paths such
    * as \b /c:/ on UNIX.
    *
    * @param url
    *        The URL to convert to a path. This is passed
    *        in as a Unicode string as the most often used
    *        pattern is to read an attribute or element value
    *        and immediately convert to a path.
    *
    * @return A string representation of the path
    */
   static std::string URLtoPath(const XMLCh *url);

   /**
    * This function will convert a path to a \b file:// URL.
    *
    * It handles Windows drive letters reasonably;
    * however, you will not be able to refer to paths such
    * as \b /c:/ on UNIX.
    *
    * @param path
    *        The path to convert to a URL.
    *
    * @return The URL string.
    */
   static std::string PathToURL(std::string path);

public:
   /**
    * Construct a new XML processor.
    *
    * Constructing an %XmlBase is not particularly useful as most
    * functionality will be in derived classes.
    *
    * @param log
    *        If this optional MessageLog is passed in, any errors
    *        encountered during processing will be logged.
    *
    * @throw XmlException
    *        When Xerces fails to initialize.
    *
    * @see XmlReader, XMLWriter
    */
   XmlBase(MessageLog* log = NULL);

   /**
    * Destroy and cleanup the XML processor.
    */
   virtual ~XmlBase();

   /**
    * Encode data in base 64 representation.
    *
    * @param data
    *        This is the 2-byte data which will be encoded.
    *
    * @param size
    *        The number of items in \e data
    *
    * @param encoding
    *        Specify additional encoding.
    *        Currently, no additional encodings are supported.
    *        An example encoding would be CCITT to generate a CCITT checksum for the data.
    *
    * @param pOutLen
    *        Optional output parameter which returns the length out the output byte array.
    *
    * @return The Unicode form of the base 64 encoded data.
    */
   static XMLByte *encodeBase64(unsigned int *data, unsigned int size, std::string encoding,
      unsigned int *pOutLen = NULL);
   
   /**
    * Decode data in base 64 representation.
    *
    * @param data
    *        This is the Unicode representation of the base 64 encoded data.
    *
    * @param size
    *        The expected number of 2-byte data items.
    *
    * @param encoding
    *        Specify additional encoding.
    *        Currently, no additional encodings are supported.
    *        An example encoding would be CCITT to verify a CCITT checksum for the data.
    *
    * @return The 2-byte array of decoded data.
    */
   static unsigned int *decodeBase64(const XMLByte *data, unsigned int size, std::string encoding);

public: // internal class
   /**
    * This class represents an exception thrown by the XML classes.
    */
   class XmlException
   {
      std::string message;

   public:
      /**
       * Create a new exception with no error message.
       */
      XmlException() {}

      /**
       * Destroy and cleanup the exception object.
       */
      virtual ~XmlException() {}

      /**
       * Create a new exception with an error message.
       *
       * @param msg
       *        The error message string.
       */
      XmlException(std::string msg) : message(msg) {}

      /**
       * Accessor for the error message.
       *
       * @return The error message string.
       */
      virtual std::string str() const { return message; }
   };

   /**
    * This is the current file format version.
    */
   static const unsigned int VERSION;

protected:
   /**
    * Log an XML exception to the message log, if one is available.
    *
    * @param exc
    *        The exception to log.
    */
   void logException(const XERCES_CPP_NAMESPACE_QUALIFIER XMLException *exc);

   /**
    * Log a Xerces DOM exception to the message log, if one is available.
    *
    * @param exc
    *        The exception to log.
    */
   void logException(const XERCES_CPP_NAMESPACE_QUALIFIER DOMException *exc);

   /**
    * Log a Xerces SAX exception to the message log, if one is available.
    *
    * @param exc
    *        The exception to log.
    *
    * @param severity
    *        A severity which is also logged. SAX exceptions may not be fatal
    *        and this is a way to inform the uses of the exception severity.
    */
   void logException(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException *exc,
                           std::string severity);

public:
   /**
    * Log a Xerces DOM error to the message log, if one is available.
    * 
    * @param exc
    *        The exception to log.
    */
   void logError(const XERCES_CPP_NAMESPACE_QUALIFIER DOMError &exc);

protected:
   /**
    * Log a string message to the message log, if one is available.
    *
    * @param msg
    *        The message to log.
    */
   void logSimpleMessage(std::string msg);

   /**
    * This is the ASCII namespace which XML data exists in.
    */
   static const char *namespaceId;

private:
   MessageLog *mpLog;
};

#endif