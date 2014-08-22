#include "CryptHash.h"
/*
#include <vector>
#include <bcrypt.h>


#pragma comment(lib, "bcrypt.lib")
//-----------------------------------------------------------------------------
// Checks if input NTSTATUS corresponds to a success code.
//-----------------------------------------------------------------------------
inline bool NtSuccess(NTSTATUS status)
{
    return (status >= 0);
}

//-----------------------------------------------------------------------------
// Class defining private copy constructor and operator=, to ban copies.
//-----------------------------------------------------------------------------
class NonCopyable
{
protected:
    NonCopyable() {}
    ~NonCopyable() {}
private:
    NonCopyable( const NonCopyable& );
    const NonCopyable& operator=( const NonCopyable& );
};

//-----------------------------------------------------------------------------
// Error occurred during cryptographic processing.
//-----------------------------------------------------------------------------
//*
class CryptException : 
    public std::runtime_error
{
public:

    // Constructs the exception with an error message and an error code.
    explicit CryptException(const std::string & message, NTSTATUS errorCode)
        : std::runtime_error( FormatErrorMessage(message, errorCode) ),
          m_errorCode(errorCode)
    {}


    // Returns the error code.
    NTSTATUS ErrorCode() const
    {
        return m_errorCode;
    }
     

    //
    // IMPLEMENTATION
    //
private:
    // Error code from Cryptography API
    NTSTATUS m_errorCode;

    // Helper method to format an error message including the error code.
    static std::string FormatErrorMessage(const std::string & message, NTSTATUS errorCode)
    {
        std::ostringstream os;
        os << message << " (NTSTATUS=0x" << std::hex << errorCode << ")";
        return os.str();
    }
};

//-----------------------------------------------------------------------------
// RAII wrapper to crypt algorithm provider
//-----------------------------------------------------------------------------
class CryptAlgorithmProvider : NonCopyable
{
public:

    // Creates a crypt algorithm provider object.
    // This can be used to create one ore more hash objects to hash some data.
    CryptAlgorithmProvider()
    {
        NTSTATUS result = ::BCryptOpenAlgorithmProvider(
            &m_alg,                     // algorithm handle
            BCRYPT_SHA1_ALGORITHM,      // hashing algorithm ID
            NULL,                    // use default provider
            0                           // default flags
            );
        if ( ! NtSuccess(result) )
		{
            //throw CryptException("Can't load default cryptographic algorithm provider.", result);
		}
    }
    

    // Releases resources
    ~CryptAlgorithmProvider()
    {
        ::BCryptCloseAlgorithmProvider(m_alg, 0);
    }


    // Gets raw handle
    BCRYPT_ALG_HANDLE Handle() const
    {
        return m_alg;
    }


    // Gets the value of a DWORD named property
    DWORD GetDWordProperty(const std::wstring & propertyName) const
    {
        DWORD propertyValue;
        DWORD resultSize;

        //
        // Get the value of the input named property
        //

        NTSTATUS result = ::BCryptGetProperty(
            Handle(), 
            propertyName.c_str(),
            reinterpret_cast<BYTE *>(&propertyValue),
            sizeof(propertyValue),
            &resultSize,
            0);
        if ( ! NtSuccess(result) )
		{
            //throw CryptException("Can't get crypt property value.", result);
		}

        return propertyValue;
    }


    //
    // IMPLEMENTATION
    //
private:
    // Handle to crypt algorithm provider
    BCRYPT_ALG_HANDLE m_alg;
};

//-----------------------------------------------------------------------------
// Crypt Hash object, used to hash data.
//-----------------------------------------------------------------------------
class CryptHashObject : NonCopyable
{
public:

    // Creates a crypt hash object.
    explicit CryptHashObject(const CryptAlgorithmProvider & provider)
        : m_hashObj( provider.GetDWordProperty(BCRYPT_OBJECT_LENGTH) )
    {
        // Create the hash object
        NTSTATUS result = ::BCryptCreateHash(
            provider.Handle(),  // handle to parent
            &m_hash,            // hash object handle
            &m_hashObj[0],   // hash object buffer pointer
            m_hashObj.size(),   // hash object buffer length
            NULL,            // no secret
            0,                  // no secret
            0                   // no flags
            );
        if ( ! NtSuccess(result) )
		{
            //throw CryptException("Can't create crypt hash object.", result);
		}
    }


    // Releases resources
    ~CryptHashObject()
    {
        ::BCryptDestroyHash(m_hash);
    }

    
    // Hashes the data in the input buffer.
    // Can be called one or more times.
    // When finished with input data, call FinishHash().
    // This method can't be called after FinisHash() is called.
    void HashData(const void * data, unsigned long length) const
    {
        // Hash this chunk of data
        NTSTATUS result = ::BCryptHashData(
            m_hash, // hash object handle
            static_cast<UCHAR *>( const_cast<void *>(data) ),    // safely remove const from buffer pointer
            length, // input buffer length in bytes
            0       // no flags
            );
        if (! NtSuccess(result) )
		{
            //throw CryptException("Can't hash data.", result);
		}
    }


    // Finalizes hash calculation.
    // After this method is called, hash value can be got using HashValue() method.
    // After this method is called, the HashData() method can't be called anymore.
    void FinishHash()
    {
        //
        // Retrieve the hash of the accumulated data
        //

        BYTE hashValue[20]; // SHA-1: 20 bytes = 160 bits

        NTSTATUS result = ::BCryptFinishHash( 
            m_hash,             // handle to hash object
            hashValue,          // output buffer for hash value
            sizeof(hashValue),  // size of this buffer
            0                   // no flags
            );
        if ( ! NtSuccess(result) )
		{
            //throw CryptException("Can't finalize hashing.", result);
		}
         

        //
        // Get the hash digest string from hash value buffer.
        //

        // Each byte --> 2 hex chars
        m_hashDigest.resize( sizeof(hashValue) * 2 );

        // Upper-case hex digits
        static const wchar_t hexDigits[] = L"0123456789ABCDEF";

        // Index to current character in destination string
        size_t currChar = 0;

        // For each byte in the hash value buffer
        for (size_t i = 0; i < sizeof(hashValue); ++i)
        {
            // high nibble
            m_hashDigest[currChar] = hexDigits[ (hashValue[i] & 0xF0) >> 4 ];
            ++currChar;

            // low nibble
            m_hashDigest[currChar] = hexDigits[ (hashValue[i] & 0x0F) ];
            ++currChar;
        }
    }


    // Gets the hash value (in hex, upper-case).
    // Call this method *after* FinishHash(), not before.
    // (If called before, an empty string is returned.)
    std::wstring HashDigest() const
    {
        return m_hashDigest;
    }


    //
    // IMPLEMENTATION
    //
private:

    // Handle to hash object
    BCRYPT_HASH_HANDLE m_hash;

    // Buffer to store hash object
    std::vector<BYTE> m_hashObj;

    // Hash digest string (hex)
    std::wstring m_hashDigest;
};

//-----------------------------------------------------------------------------
// Wrapper around C FILE *, for reading binary data from file.
//-----------------------------------------------------------------------------
class FileReader : NonCopyable
{
public:

    // Opens the specified file.
    explicit FileReader(const std::wstring & filename)
    {
        if ( _wfopen_s(&m_file, filename.c_str(), L"rb") != 0)
        {
            throw std::runtime_error("Can't open file for reading.");
        }
    }


    // Closes the file.
    ~FileReader()
    {
        if (m_file != NULL)
            fclose(m_file);
    }


    // End Of File reached?
    bool EoF() const
    {
        return feof(m_file) ? true : false;
    }


    // Reads bytes from file to a memory buffer.
    // Returns the number of bytes actually read.
    int Read(void * buffer, int bufferSize)
    {
        return fread(buffer, 1, bufferSize, m_file);
    }


    //
    // IMPLEMENTATION
    //
private:
    // Raw C file handle
    FILE * m_file;
};



//-----------------------------------------------------------------------------
// Hashes a file with SHA-1.
// Returns the hash digest, in hex.
//-----------------------------------------------------------------------------
std::wstring HashFileSHA1(const std::wstring & filename)
{
    // Create the algorithm provider for SHA-1 hashing
    CryptAlgorithmProvider sha1;

    // Create the hash object for the particular hashing
    CryptHashObject hasher(sha1);

    // Object to read data from file
    FileReader file(filename);

    // Read buffer
    std::vector<BYTE> buffer(4*1024);   // 4 KB buffer

    // Reading loop
    while ( ! file.EoF() )
    {
        // Read a chunk of data from file to memory buffer
        int readBytes = file.Read(&buffer[0], buffer.size());

        // Hash this chunk of data
        hasher.HashData(&buffer[0], readBytes);
    }

    // Finalize hashing
    hasher.FinishHash();

    // Return hash digest
    return hasher.HashDigest();
}

//-----------------------------------------------------------------------------
// Hashes a string with SHA-1.
// Returns the hash digest, in hex.
//-----------------------------------------------------------------------------
std::wstring HashStringSHA1(const std::wstring & data)
{
    // Create the algorithm provider for SHA-1 hashing
    CryptAlgorithmProvider sha1;

    // Create the hash object for the particular hashing
    CryptHashObject hasher(sha1);

    // Hash this chunk of data
	hasher.HashData(&data[0], data.size());

    // Finalize hashing
    hasher.FinishHash();

    // Return hash digest
    return hasher.HashDigest();
}

//-----------------------------------------------------------------------------
// Hashes a string with SHA-1.
// Returns the hash digest, in hex.
//-----------------------------------------------------------------------------
CString CryptHash::HashStringSHA1(LPCSTR data)
{
    // Create the algorithm provider for SHA-1 hashing
    CryptAlgorithmProvider sha1;

    // Create the hash object for the particular hashing
    CryptHashObject hasher(sha1);

    // Hash this chunk of data
	hasher.HashData(data, strlen(data));

    // Finalize hashing
    hasher.FinishHash();

    // Return hash digest
	CString srtHex;
	srtHex = hasher.HashDigest().c_str();
    return srtHex;
}
*/

class SHA1
{

    public:

        SHA1();
        virtual ~SHA1();

        /*
         *  Re-initialize the class
         */
        void Reset();

        /*
         *  Returns the message digest
         */
        bool Result(unsigned *message_digest_array);

        /*
         *  Provide input to SHA1
         */
        void Input( const unsigned char *message_array,
                    unsigned            length);
        void Input( const char  *message_array,
                    unsigned    length);
        void Input(unsigned char message_element);
        void Input(char message_element);

    private:

        /*
         *  Process the next 512 bits of the message
         */
        void ProcessMessageBlock();

        /*
         *  Pads the current message block to 512 bits
         */
        void PadMessage();

        /*
         *  Performs a circular left shift operation
         */
        inline unsigned CircularShift(int bits, unsigned word);

        unsigned H[5];                      // Message digest buffers

        unsigned Length_Low;                // Message length in bits
        unsigned Length_High;               // Message length in bits

        unsigned char Message_Block[64];    // 512-bit message blocks
        int Message_Block_Index;            // Index into message block array

        bool Computed;                      // Is the digest computed?
        bool Corrupted;                     // Is the message digest corruped?
    
};
SHA1::SHA1()
{
    Reset();
}

/*  
 *  ~SHA1
 *
 *  Description:
 *      This is the destructor for the sha1 class
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
SHA1::~SHA1()
{
    // The destructor does nothing
}

/*  
 *  Reset
 *
 *  Description:
 *      This function will initialize the sha1 class member variables
 *      in preparation for computing a new message digest.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
void SHA1::Reset()
{
    Length_Low          = 0;
    Length_High         = 0;
    Message_Block_Index = 0;

    H[0]        = 0x67452301;
    H[1]        = 0xEFCDAB89;
    H[2]        = 0x98BADCFE;
    H[3]        = 0x10325476;
    H[4]        = 0xC3D2E1F0;

    Computed    = false;
    Corrupted   = false;
}

/*  
 *  Result
 *
 *  Description:
 *      This function will return the 160-bit message digest into the
 *      array provided.
 *
 *  Parameters:
 *      message_digest_array: [out]
 *          This is an array of five unsigned integers which will be filled
 *          with the message digest that has been computed.
 *
 *  Returns:
 *      True if successful, false if it failed.
 *
 *  Comments:
 *
 */
bool SHA1::Result(unsigned *message_digest_array)
{
    int i;                                  // Counter

    if (Corrupted)
    {
        return false;
    }

    if (!Computed)
    {
        PadMessage();
        Computed = true;
    }

    for(i = 0; i < 5; i++)
    {
        message_digest_array[i] = H[i];
    }

    return true;
}

/*  
 *  Input
 *
 *  Description:
 *      This function accepts an array of octets as the next portion of
 *      the message.
 *
 *  Parameters:
 *      message_array: [in]
 *          An array of characters representing the next portion of the
 *          message.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
void SHA1::Input(   const unsigned char *message_array,
                    unsigned            length)
{
    if (!length)
    {
        return;
    }

    if (Computed || Corrupted)
    {
        Corrupted = true;
        return;
    }

    while(length-- && !Corrupted)
    {
        Message_Block[Message_Block_Index++] = (*message_array & 0xFF);

        Length_Low += 8;
        Length_Low &= 0xFFFFFFFF;               // Force it to 32 bits
        if (Length_Low == 0)
        {
            Length_High++;
            Length_High &= 0xFFFFFFFF;          // Force it to 32 bits
            if (Length_High == 0)
            {
                Corrupted = true;               // Message is too long
            }
        }

        if (Message_Block_Index == 64)
        {
            ProcessMessageBlock();
        }

        message_array++;
    }
}

/*  
 *  Input
 *
 *  Description:
 *      This function accepts an array of octets as the next portion of
 *      the message.
 *
 *  Parameters:
 *      message_array: [in]
 *          An array of characters representing the next portion of the
 *          message.
 *      length: [in]
 *          The length of the message_array
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
void SHA1::Input(   const char  *message_array,
                    unsigned    length)
{
    Input((unsigned char *) message_array, length);
}

/*  
 *  Input
 *
 *  Description:
 *      This function accepts a single octets as the next message element.
 *
 *  Parameters:
 *      message_element: [in]
 *          The next octet in the message.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
void SHA1::Input(unsigned char message_element)
{
    Input(&message_element, 1);
}

/*  
 *  Input
 *
 *  Description:
 *      This function accepts a single octet as the next message element.
 *
 *  Parameters:
 *      message_element: [in]
 *          The next octet in the message.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
void SHA1::Input(char message_element)
{
    Input((unsigned char *) &message_element, 1);
}

/*  
 *  ProcessMessageBlock
 *
 *  Description:
 *      This function will process the next 512 bits of the message
 *      stored in the Message_Block array.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      Many of the variable names in this function, especially the single
 *      character names, were used because those were the names used
 *      in the publication.
 *
 */
void SHA1::ProcessMessageBlock()
{
    const unsigned K[] =    {               // Constants defined for SHA-1
                                0x5A827999,
                                0x6ED9EBA1,
                                0x8F1BBCDC,
                                0xCA62C1D6
                            };
    int         t;                          // Loop counter
    unsigned    temp;                       // Temporary word value
    unsigned    W[80];                      // Word sequence
    unsigned    A, B, C, D, E;              // Word buffers

    /*
     *  Initialize the first 16 words in the array W
     */
    for(t = 0; t < 16; t++)
    {
        W[t] = ((unsigned) Message_Block[t * 4]) << 24;
        W[t] |= ((unsigned) Message_Block[t * 4 + 1]) << 16;
        W[t] |= ((unsigned) Message_Block[t * 4 + 2]) << 8;
        W[t] |= ((unsigned) Message_Block[t * 4 + 3]);
    }

    for(t = 16; t < 80; t++)
    {
       W[t] = CircularShift(1,W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
    }

    A = H[0];
    B = H[1];
    C = H[2];
    D = H[3];
    E = H[4];

    for(t = 0; t < 20; t++)
    {
        temp = CircularShift(5,A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = CircularShift(30,B);
        B = A;
        A = temp;
    }

    for(t = 20; t < 40; t++)
    {
        temp = CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[1];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = CircularShift(30,B);
        B = A;
        A = temp;
    }

    for(t = 40; t < 60; t++)
    {
        temp = CircularShift(5,A) +
               ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = CircularShift(30,B);
        B = A;
        A = temp;
    }

    for(t = 60; t < 80; t++)
    {
        temp = CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[3];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = CircularShift(30,B);
        B = A;
        A = temp;
    }

    H[0] = (H[0] + A) & 0xFFFFFFFF;
    H[1] = (H[1] + B) & 0xFFFFFFFF;
    H[2] = (H[2] + C) & 0xFFFFFFFF;
    H[3] = (H[3] + D) & 0xFFFFFFFF;
    H[4] = (H[4] + E) & 0xFFFFFFFF;

    Message_Block_Index = 0;
}

/*  
 *  PadMessage
 *
 *  Description:
 *      According to the standard, the message must be padded to an even
 *      512 bits.  The first padding bit must be a '1'.  The last 64 bits
 *      represent the length of the original message.  All bits in between
 *      should be 0.  This function will pad the message according to those
 *      rules by filling the message_block array accordingly.  It will also
 *      call ProcessMessageBlock() appropriately.  When it returns, it
 *      can be assumed that the message digest has been computed.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
void SHA1::PadMessage()
{
    /*
     *  Check to see if the current message block is too small to hold
     *  the initial padding bits and length.  If so, we will pad the
     *  block, process it, and then continue padding into a second block.
     */
    if (Message_Block_Index > 55)
    {
        Message_Block[Message_Block_Index++] = 0x80;
        while(Message_Block_Index < 64)
        {
            Message_Block[Message_Block_Index++] = 0;
        }

        ProcessMessageBlock();

        while(Message_Block_Index < 56)
        {
            Message_Block[Message_Block_Index++] = 0;
        }
    }
    else
    {
        Message_Block[Message_Block_Index++] = 0x80;
        while(Message_Block_Index < 56)
        {
            Message_Block[Message_Block_Index++] = 0;
        }

    }

    /*
     *  Store the message length as the last 8 octets
     */
    Message_Block[56] = (Length_High >> 24) & 0xFF;
    Message_Block[57] = (Length_High >> 16) & 0xFF;
    Message_Block[58] = (Length_High >> 8) & 0xFF;
    Message_Block[59] = (Length_High) & 0xFF;
    Message_Block[60] = (Length_Low >> 24) & 0xFF;
    Message_Block[61] = (Length_Low >> 16) & 0xFF;
    Message_Block[62] = (Length_Low >> 8) & 0xFF;
    Message_Block[63] = (Length_Low) & 0xFF;

    ProcessMessageBlock();
}


/*  
 *  CircularShift
 *
 *  Description:
 *      This member function will perform a circular shifting operation.
 *
 *  Parameters:
 *      bits: [in]
 *          The number of bits to shift (1-31)
 *      word: [in]
 *          The value to shift (assumes a 32-bit integer)
 *
 *  Returns:
 *      The shifted value.
 *
 *  Comments:
 *
 */
unsigned SHA1::CircularShift(int bits, unsigned word)
{
    return ((word << bits) & 0xFFFFFFFF) | ((word & 0xFFFFFFFF) >> (32-bits));
}

CString CryptHash::HashStringSHA1(LPCSTR data)
{
    // Create the algorithm provider for SHA-1 hashing
    SHA1 sha1;

	// Hash this chunk of data
	sha1.Input(data, strlen(data));

	unsigned message_digest[5];
	CString srtHex;
	if (sha1.Result(message_digest))
    {
        srtHex.Format(_T("%08X%08X%08X%08X%08X"),
                message_digest[0],
                message_digest[1],
                message_digest[2],
                message_digest[3],
                message_digest[4]);
    }
    return srtHex;
}