#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <sstream>

/*
 * This program parses Apache's Common Log Format
 * (http://httpd.apache.org/docs/1.3/logs.html#common)
 * and Combined Log Format into
 * IP, client-identity, user-id, unix-epoch-seconds, http-method, http-resource, http-protocol, status-code, returned-size[, referer[, user-agent]]
 * separated by tabs.
 *
 * It takes its input via stdin and writes to stdout.
 */

using std::vector;
using std::string;
using std::cout;
using std::cin;
using std::stringstream;

/*
 * Parses calendar input into tm, converts to time_t, and reformats as a str.
 * logtime should be the format: day/month/year:hour:minute:second zone
 * day = 2*digit
 * month = 3*letter
 * year = 4*digit
 * hour = 2*digit
 * minute = 2*digit
 * second = 2*digit
 * zone = (`+' | `-') 4*digit
 *
 * e.g. 04/Apr/2012:10:37:29 -0500
 */
string logtimeToUnix(const string& logtime) {
  struct tm tm;
  time_t t;
  if (strptime(logtime.c_str(), "%d/%b/%Y:%H:%M:%S %Z", &tm) == NULL)
    return "-";

  tm.tm_isdst = 0; // Force dst off
  // Parse the timezone, the five digits start with the sign at idx 21.
  int hours = 10*(logtime[22] - '0') + logtime[23] - '0';
  int mins = 10*(logtime[24] - '0') + logtime[25] - '0';
  int off_secs = 60*60*hours + 60*mins;
  if (logtime[21] == '-')
    off_secs *= -1;

  t = mktime(&tm);
  if (t == -1)
    return "-";
  t -= timezone; // Local timezone
  t += off_secs;

  string retval;
  stringstream stream;
  stream << t;
  stream >> retval;
  return retval;
}

/*
 * Simple FSM, progresses through each section of the CLF.
 * State 1: IP
 * State 2: Client identity
 * State 3: User
 * State 4: Time
 * State 5: URL pieces
 * State 6: Code
 * State 7: Returned content size
 * State 8 (optional): Referrer
 * State 9 (optional): User-agent.
 */
void scanCLF(const string& line) {
  vector<string> tokens;
  tokens.reserve(16);
  string token("");
  unsigned state = 1;
  for (string::const_iterator it = line.begin(); it != line.end(); ++it) {
    switch(state) {
      case 1:
      case 2:
      case 3:
      case 6:
      case 7:
        if (*it != ' ') {
          token += *it;
        } else if (!token.empty()) {
          tokens.push_back(token);
          token.clear();
          ++state;
        }
        // Optional case 8 not reached:
        if (it+1 == line.end() && !token.empty()) {
          tokens.push_back(token);
        }
        break;
      case 4:
        if (*it != '[' && *it != ']') {
          token += *it;
        } else if (*it == ']') {
          tokens.push_back(logtimeToUnix(token));
          token.clear();
          ++state;
        }
        break;
      case 5:
        if (*it != '"') {
          if (*it != ' ') {
            token += *it;
          } else if (!token.empty()) {
            tokens.push_back(token);
            token.clear();
          }
        } else if (!token.empty()) {
          tokens.push_back(token);
          token.clear();
          ++state;
        }
        break;
      case 8:
      case 9:
        if (*it != '"' && (*it != ' ' || !token.empty())) {
          token += *it;
        } else if (!token.empty()) {
          tokens.push_back(token);
          token.clear();
          ++state;
        }
        break;
    }
  }
  for (unsigned i = 0; i < tokens.size(); ++i) {
    cout << tokens[i] << "\t";
  }
  cout << std::endl;
}

int main() {
  string line;
  // This often makes input much faster but creates
  // a memory leak because standard streams are *never*
  // destroyed as per the standard.
  cin.sync_with_stdio(false);
  while (std::getline(cin, line)) {
    scanCLF(line);
  }
  return 0;
}
