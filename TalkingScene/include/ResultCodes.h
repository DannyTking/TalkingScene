#ifndef _RESULT_CODES_H
#define _RESULT_CODES_H

namespace Walaber
{
    enum ResultCode
    {
        ResultCode_Invalid=0,
        ResultCode_OK,
        ResultCode_Error,				// generic error.
        ResultCode_ErrorFileNotFound,	// file I/O error: file not found
        ResultCode_ErrorXMLParseError	// error parsing XML data (syntax error, etc)
    };
}


#endif // _RESULT_CODES_H_
