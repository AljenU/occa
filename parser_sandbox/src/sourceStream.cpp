/* The MIT License (MIT)
 *
 * Copyright (c) 2014-2018 David Medina and Tim Warburton
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 */
#include "sourceStream.hpp"
#include "occa/tools/string.hpp"

namespace occa {
  namespace lang {
    int getEncodingType(const std::string &str) {
      int encoding      = 0;
      int encodingCount = 0;
      const char *c = str.c_str();
      while (*c) {
        int newEncoding = 0;
        switch (*c) {
        case 'u': {
          if (c[1] == '8') {
            newEncoding = encodingType::u8;
            ++c;
          } else {
            newEncoding = encodingType::u;
          }
          break;
        }
        case 'U':
          newEncoding = encodingType::U; break;
        case 'L':
          newEncoding = encodingType::L; break;
        case 'R':
          newEncoding = encodingType::R; break;
        }
        if (!newEncoding ||
            (newEncoding & encoding)) {
          return encodingType::none;
        }
        encoding |= newEncoding;
        ++encodingCount;
        ++c;
      }
      if ((encodingCount == 1) ||
          ((encodingCount == 2) && (encoding & encodingType::R))) {
        return encoding;
      }
      return encodingType::none;
    }

    int getCharacterEncoding(const std::string &str) {
      const int encoding = getEncodingType(str);
      if (!encoding ||
          (encoding & (encodingType::u8 |
                       encodingType::R))) {
        return encodingType::none;
      }
      return encoding;
    }

    int getStringEncoding(const std::string &str) {
      return getEncodingType(str);
    }

    sourceStream::sourceStream(const char *root) :
      origin(NULL, filePosition(root)),
      fp(origin.position) {
      pushSource(true, NULL, origin.position);
    }

    sourceStream::sourceStream(file_t *file_,
                               const char *root) :
      origin(file_, filePosition(root)),
      fp(origin.position) {
      pushSource(true, file_, origin.position);
    }

    sourceStream::sourceStream(const sourceStream &stream) :
      tokenStreamWithMap(stream),
      origin(stream.origin),
      fp(origin.position),
      stack(stream.stack),
      sourceStack(stream.sourceStack) {}

    sourceStream& sourceStream::operator = (const sourceStream &stream) {
      copyStreamTransforms(stream);
      origin      = stream.origin;
      stack       = stream.stack;
      sourceStack = stream.sourceStack;
      return *this;
    }

    sourceStream::~sourceStream() {}

    void sourceStream::preprint(std::ostream &out) {
      origin.preprint(out);
    }

    void sourceStream::postprint(std::ostream &out) {
      origin.postprint(out);
    }

    void sourceStream::setLine(const int line) {
      fp.line = line;
    }

    bool sourceStream::isEmpty() {
      return (*fp.pos == '\0');
    }

    void sourceStream::pushSource(const bool fromInclude,
                                  file_t *file,
                                  const filePosition &position) {
      sourceStack.push_back(stack);
      origin.push(fromInclude,
                  file,
                  position);
    }

    void sourceStream::popSource() {
      OCCA_ERROR("Unable to call sourceStream::popSource()",
                 sourceStack.size() > 0);
      stack = sourceStack.back();
      sourceStack.pop_back();
    }

    void sourceStream::push() {
      stack.push_back(origin);
    }

    void sourceStream::pop(const bool rewind) {
      if (stack.size() == 0) {
        printError("Trying to pop() without a stack");
        return;
      }
      if (rewind) {
        origin = stack.back();
      }
      stack.pop_back();
    }

    void sourceStream::popAndRewind() {
      pop(true);
    }

    std::string sourceStream::str() {
      if (stack.size() == 0) {
        printError("Not able to str() without a stack");
        return "";
      }
      fileOrigin last = stack.back();
      return std::string(last.position.pos,
                         fp.pos - last.position.pos);
    }

    void sourceStream::countSkippedLines() {
      if (stack.size() == 0) {
        printError("Not able to countSkippedLines() without a stack");
        return;
      }
      fileOrigin last = stack.back();
      if (last.file != origin.file) {
        printError("Trying to countSkippedLines() across different files");
        return;
      }
      const char *pos = last.position.pos;
      const char *end = fp.pos;
      while (pos < end) {
        if (*pos == '\\') {
          if (fp.pos[1] == '\n') {
            fp.lineStart = fp.pos + 2;
            ++fp.line;
          }
          pos += 1 + (pos[1] != '\0');
          continue;
        }
        if (*pos == '\n') {
          fp.lineStart = fp.pos + 1;
          ++fp.line;
        }
        ++pos;
      }
    }

    void sourceStream::skipTo(const char delimiter) {
      while (*fp.pos != '\0') {
        if (*fp.pos == '\\') {
          if (fp.pos[1] == '\n') {
            fp.lineStart = fp.pos + 2;
            ++fp.line;
          }
          fp.pos += 1 + (fp.pos[1] != '\0');
          continue;
        }
        if (*fp.pos == delimiter) {
          return;
        }
        if (*fp.pos == '\n') {
          fp.lineStart = fp.pos + 1;
          ++fp.line;
        }
        ++fp.pos;
      }
    }

    void sourceStream::skipTo(const char *delimiters) {
      while (*fp.pos != '\0') {
        if (*fp.pos == '\\') {
          if (fp.pos[1] == '\n') {
            fp.lineStart = fp.pos + 2;
            ++fp.line;
          }
          fp.pos += 1 + (fp.pos[1] != '\0');
          continue;
        }
        if (lex::charIsIn(*fp.pos, delimiters)) {
          return;
        }
        if (*fp.pos == '\n') {
          fp.lineStart = fp.pos + 1;
          ++fp.line;
        }
        ++fp.pos;
      }
    }

    void sourceStream::skipFrom(const char *delimiters) {
      while (*fp.pos != '\0') {
        if (*fp.pos == '\\') {
          if (fp.pos[1] == '\n') {
            fp.lineStart = fp.pos + 2;
            ++fp.line;
          }
          fp.pos += 1 + (fp.pos[1] != '\0');
          continue;
        }
        if (!lex::charIsIn(*fp.pos, delimiters)) {
          break;
        }
        if (*fp.pos == '\n') {
          fp.lineStart = fp.pos + 1;
          ++fp.line;
        }
        ++fp.pos;
      }
    }

    void sourceStream::skipWhitespace() {
      skipFrom(charcodes::whitespaceNoNewline);
    }

    int sourceStream::peek() {
      int type = shallowPeek();
      if (type & tokenType::identifier) {
        type = peekForIdentifier();
      } else if (type & tokenType::op) {
        type = peekForOperator();
      }
      return type;
    }

    int sourceStream::shallowPeek() {
      skipWhitespace();

      const char c = *fp.pos;
      if (c == '\0') {
        return tokenType::none;
      }
      if (lex::charIsIn(c, charcodes::identifierStart)) {
        return tokenType::identifier;
      }
      // Primitive must be checked before operators since
      //   it can start with a . (for example, .01)
      const char *pos = fp.pos;
      if (primitive::load(pos, false).type != occa::primitiveType::none) {
        return tokenType::primitive;
      }
      if (lex::charIsIn(c, charcodes::operators)) {
        return tokenType::op;
      }
      if (c == '\n') {
        return tokenType::newline;
      }
      if (c == '"') {
        return tokenType::string;
      }
      if (c == '\'') {
        return tokenType::char_;
      }
      return tokenType::none;
    }

    int sourceStream::peekForIdentifier() {
      push();
      ++fp.pos;
      skipFrom(charcodes::identifier);
      const std::string identifier = str();
      int type = shallowPeek();
      popAndRewind();

      // [u8R]"foo" or [u]'foo'
      if (type & tokenType::string) {
        const int encoding = getStringEncoding(identifier);
        if (encoding) {
          return (tokenType::string |
                  (encoding << tokenType::encodingShift));
        }
      }
      if (type & tokenType::char_) {
        const int encoding = getCharacterEncoding(identifier);
        if (encoding) {
          return (tokenType::char_ |
                  (encoding << tokenType::encodingShift));
        }
      }
      return tokenType::identifier;
    }

    int sourceStream::peekForOperator() {
      push();
      fileOrigin tokenOrigin = origin;
      operatorTrie &operators = getOperators();
      operatorTrie::result_t result = operators.getLongest(fp.pos);
      if (!result.success()) {
        printError("Not able to parse operator");
        popAndRewind();
        return tokenType::none;
      }
      const operator_t &op = *(result.value());
      if (op.opType & operatorType::comment) {
        pop();
        if (op.opType == operatorType::lineComment) {
          return skipLineCommentAndPeek();
        }
        else if (op.opType == operatorType::blockCommentStart) {
          return skipBlockCommentAndPeek();
        }
        else {
          printError("Couldn't find an opening /*");
          return peek();
        }
      }
      popAndRewind();
      return tokenType::op;
    }

    int sourceStream::peekForHeader() {
      int type = shallowPeek();
      if (type & tokenType::op) {
        push();
        operatorTrie &operators = getOperators();
        operatorTrie::result_t result = operators.getLongest(fp.pos);
        popAndRewind();
        if (result.success() &&
            (result.value()->opType & operatorType::lessThan)) {
          return tokenType::systemHeader;
        }
      }
      else if (type & tokenType::string) {
        return tokenType::header;
      }
      return tokenType::none;
    }

    void sourceStream::getIdentifier(std::string &value) {
      if (!lex::charIsIn(*fp.pos, charcodes::identifierStart)) {
        return;
      }
      push();
      ++fp.pos;
      skipFrom(charcodes::identifier);
      value = str();
      pop();
    }

    void sourceStream::getString(std::string &value,
                                 const int encoding) {
      if (encoding & encodingType::R) {
        getRawString(value);
        return;
      }
      if (*fp.pos != '"') {
        return;
      }
      push();
      ++fp.pos;
      push();
      skipTo("\"\n");
      if (*fp.pos == '\n') {
        pop();
        popAndRewind();
        return;
      }
      value = str();
      pop();
      pop();
      ++fp.pos;
    }

    void sourceStream::getRawString(std::string &value) {
      // TODO: Keep the delimiter(s)
      if (*fp.pos != '"') {
        return;
      }
      push();
      ++fp.pos; // Skip "
      push();

      // Find delimiter
      skipTo("(\n");
      if (*fp.pos == '\n') {
        pop();
        popAndRewind();
        return;
      }

      // Find end pattern
      std::string end;
      end += ')';
      end += str();
      end += '"';
      pop();
      ++fp.pos; // Skip (
      push();

      // Find end match
      const int chars = (int) end.size();
      const char *m   = end.c_str();
      int mi;
      while (*fp.pos != '\0') {
        for (mi = 0; mi < chars; ++mi) {
          if (fp.pos[mi] != m[mi]) {
            break;
          }
        }
        if (mi == chars) {
          break;
        }
        if (*fp.pos == '\n') {
          fp.lineStart = fp.pos + 1;
          ++fp.line;
        }
        ++fp.pos;
      }

      // Make sure we found delimiter
      if (*fp.pos == '\0') {
        pop();
        popAndRewind();
        return;
      }
      value = str();
      fp.pos += chars;
    }

    int sourceStream::skipLineCommentAndPeek() {
      skipTo('\n');
      return (fp.pos
              ? tokenType::newline
              : tokenType::none);
    }

    int sourceStream::skipBlockCommentAndPeek() {
      while (*fp.pos != '\0') {
        skipTo('*');
        if (*fp.pos == '*') {
          ++fp.pos;
          if (*fp.pos == '/') {
            ++fp.pos;
            skipWhitespace();
            return peek();
          }
        }
      }
      return tokenType::none;
    }

    token_t* sourceStream::_getToken() {
      skipWhitespace();

      // Check if file finished
      bool finishedSource = false;
      while ((*fp.pos == '\0') && stack.size()) {
        popSource();
        skipWhitespace();
        finishedSource = true;
      }
      if (finishedSource) {
        return new newlineToken(origin);
      }
      if ((*fp.pos == '\0') && !stack.size()) {
        return NULL;
      }

      int type = peek();
      if (type & tokenType::identifier) {
        return getIdentifierToken();
      }
      if (type & tokenType::primitive) {
        return getPrimitiveToken();
      }
      if (type & tokenType::op) {
        return getOperatorToken();
      }
      if (type & tokenType::newline) {
        ++fp.pos;
        return new newlineToken(origin);
      }
      if (type & tokenType::char_) {
        return getCharToken(tokenType::getEncoding(type));
      }
      if (type & tokenType::string) {
        return getStringToken(tokenType::getEncoding(type));
      }

      printError("Not able to create token for:");
      return NULL;
    }

    token_t* sourceStream::getIdentifierToken() {
      fileOrigin tokenOrigin = origin;
      if (!lex::charIsIn(*fp.pos, charcodes::identifierStart)) {
        printError("Not able to parse identifier");
        return NULL;
      }
      std::string value;
      getIdentifier(value);
      return new identifierToken(tokenOrigin,
                                 value);
    }

    token_t* sourceStream::getPrimitiveToken() {
      fileOrigin tokenOrigin = origin;
      push();
      primitive value = primitive::load(fp.pos);
      if (value.isNaN()) {
        printError("Not able to parse primitive");
        popAndRewind();
        return NULL;
      }
      const std::string strValue = str();
      countSkippedLines();
      pop();
      return new primitiveToken(tokenOrigin,
                                value,
                                strValue);
    }

    token_t* sourceStream::getOperatorToken() {
      fileOrigin tokenOrigin = origin;
      operatorTrie &operators = getOperators();
      operatorTrie::result_t result = operators.getLongest(fp.pos);
      if (!result.success()) {
        printError("Not able to parse operator");
        return NULL;
      }
      fp.pos += result.length; // Skip operator
      return new operatorToken(tokenOrigin,
                               *(result.value()));
    }

    token_t* sourceStream::getStringToken(const int encoding) {
      fileOrigin tokenOrigin = origin;
      if (encoding) {
        std::string encodingStr;
        getIdentifier(encodingStr);
      }
      if (*fp.pos != '"') {
        printError("Not able to parse string");
        return NULL;
      }
      const char *start = fp.pos;
      std::string value, udf;
      getString(value, encoding);
      if (fp.pos == start) {
        printError("Not able to find closing \"");
        return NULL;
      }
      if (*fp.pos == '_') {
        getIdentifier(udf);
      }
      return new stringToken(tokenOrigin,
                             encoding, value, udf);
    }

    token_t* sourceStream::getCharToken(const int encoding) {
      fileOrigin tokenOrigin = origin;
      if (encoding) {
        std::string encodingStr;
        getIdentifier(encodingStr);
      }
      if (*fp.pos != '\'') {
        printError("Not able to parse char");
        return NULL;
      }
      ++fp.pos; // Skip '
      push();
      skipTo("'\n");
      if (*fp.pos == '\n') {
        printError("Not able to find closing '");
        popAndRewind();
        return NULL;
      }
      const std::string value = str();
      ++fp.pos;
      pop();

      std::string udf;
      if (*fp.pos == '_') {
        getIdentifier(udf);
      }
      return new charToken(tokenOrigin,
                           encoding, value, udf);
    }

    token_t* sourceStream::getHeaderToken() {
      fileOrigin tokenOrigin = origin;
      int type = shallowPeek();
      if (type & tokenType::op) {
        ++fp.pos; // Skip <
        push();
        skipTo(">\n");
        if (*fp.pos == '\n') {
          printError("Not able to find closing >");
          popAndRewind();
          return NULL;
        }
        token_t *token = new headerToken(tokenOrigin,
                                         true, str());
        ++fp.pos; // Skip >
        pop();
        return token;
      }
      if (!(type & tokenType::string)) {
        printError("Not able to parse header");
        return NULL;
      }
      std::string value;
      getString(value);
      return new headerToken(tokenOrigin,
                             false, value);
    }
  }
}
