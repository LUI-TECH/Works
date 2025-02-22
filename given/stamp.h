

enum MessageStatus {
  INVALID_HEADER=-1,
  WRONG_RECIPIENT=-2,
  INVALID_MESSAGE_DIGEST=-3,
  INVALID_HEADER_DIGEST=-4,
  VALID_EMAIL=1
};

/* pre-supplied helper function */
/* generates the SHA1 hash of input string text into output parameter digest */
void text_to_SHA1_digest(const char *text, char *digest);
int leading_zeros(std::string digest);
bool file_to_SHA1_digest(std::string filename, char * digest);
void error(char * digest);
bool make_header(const std::string recipient, const std::string filename, char * header);
MessageStatus check_header(const std::string email_address, char * header, std::string filename);
