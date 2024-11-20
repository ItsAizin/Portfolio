#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <bitset>
using namespace std;

// Helper function to calculate the complement of an integer
int invert_num(int val) {
    string complementString = bitset<8>(val).to_string();
    for (char &ch : complementString) {
        if (ch == '0') { 
                ch = '1';
        } else if (ch == '1') {
                ch = '0';
        }
    }
    bitset<32> binaryBits(complementString);
    return binaryBits.to_ulong();
}

// Helper function to check the validity of inputs
int validity(const string& input) {
    int valid = 1;
    int value;
    int periodCounter = 0;
    int periodPosition[4];
    periodPosition[0] = -1;
	for (int i = 0; i < input.size(); i++) {
        if (input[i] == '.') {
                periodCounter++;
                // save the period positions
                periodPosition[periodCounter] = i;
        }
	}
	// To check whether the provided IP has 4 octets
	if (periodCounter < 3) {
	    valid = 0;
	    return valid;
	}
	for (int i = 0; i < 4; i++) {
        if (input[i] == '.') {
                periodCounter++;
                if (periodCounter > 3) {
                        valid = 0;
                        return valid;
                }
                // save the period positions
                periodPosition[periodCounter] = i;
        }
	    // extracting the last octet
        if (((periodPosition[i+1] - periodPosition[i] - 1) == 0) or (periodPosition[i]+1 == input.size())){
                valid = 0;
                return valid;
        }
		// To check whether the octets are in the range 0-255
        if ((value > 255) and (value < 0)) {
                valid = 0;
                return valid;
        }
	}
	return valid;
}

// Helper function to parse IP address from a string.
vector<int> parseOctets(const string& input) {
    vector<int> values(4);
    int periodCounter = 0;
    int periodPosition[4];
    periodPosition[0] = -1;
	for (int i = 0; i < input.size(); i++) {

        if (i == 3) {
                value = stoi(input.substr(periodPosition[i]+1));
        }
        else {
                value = stoi(input.substr(periodPosition[i]+1, (periodPosition[i+1] - periodPosition[i] - 1)));
        }

	}
	for (int i = 0; i < 4; i++) {
        if (i == 3) {
                values[i] = stoi(input.substr(periodPosition[i]+1));
        }
        else {
                values[i] = stoi(input.substr(periodPosition[i]+1, (periodPosition[i+1] - periodPosition[i] - 1)));
        }
 
	}
	return values;
}

// Helper function to obtain subnetmask from integer value
vector<int> parseSM(int val) {
    vector<int> values(4);
    string binaryString(val, '1');
    binaryString.resize(32, '0');
    string substr;
    for (int i = 0; i < 4; i++) {
        substr = binaryString.substr(8*i, 8);
        bitset<32> binaryBits(substr);
        values[i] = binaryBits.to_ulong();
	}
    return values;
}

class IPAddress {
	private:
		vector<int> ipOctets;
		vector<int> smOctets;
	public:
		IPAddress(const vector<int>& ip, const vector<int>& subnetMask) : ipOctets(ip), smOctets(subnetMask) {
			if (ipOctets.size() != 4 || smOctets.size() != 4) {
				cout << "Both IP address and subnet mask must have 4 octets.";
			}
		}
		vector<int> calculateNetworkAddress() {
			vector<int> networkAddress(4);
			for (int i = 0; i < 4; ++i) {
				networkAddress[i] = ipOctets[i] & smOctets[i];
			}
			return networkAddress;
		}
		vector<int> calculateBroadcastAddress() {
			vector<int> broadcastAddress(4);
			for (int i = 0; i < 4; ++i) {
				broadcastAddress[i] = ipOctets[i] | invert_num(smOctets[i]);
			}
			return broadcastAddress;
		}
		pair<vector<int>, vector<int>> calculateValidIPAddressRange() {
			vector<int> networkAddress = calculateNetworkAddress();
			vector<int> broadcastAddress = calculateBroadcastAddress();
			return make_pair(networkAddress, broadcastAddress);
		}
};

int main() {
    string ipAddressStr;
    int subnetMaskInt;
    cout << "Enter an IPv4 address (xxx.xxx.xxx.xxx): ";
    cin >> ipAddressStr;
    int valid = validity(ipAddressStr);
    if (valid == 0) {
        cout << "Invalid inputs" << endl;
        return 0;
    }
    vector<int> ipAddress = parseOctets(ipAddressStr);
    cout << "Enter a subnet mask [1-32]: ";
    cin >> subnetMaskInt;
    if ((subnetMaskInt < 1) or (subnetMaskInt > 32)) {
        cout << "Invalid subnet mask" << endl;
        return 0;
    }
    vector<int> subnetMask = parseSM(subnetMaskInt);
    IPAddress ip(ipAddress, subnetMask);
    vector<int> networkAddress = ip.calculateNetworkAddress();
    vector<int> broadcastAddress = ip.calculateBroadcastAddress();
    pair<vector<int>, vector<int>> validRange = ip.calculateValidIPAddressRange();
    cout << "Network Address: ";
    for (int i = 0; i < 4; ++i) {
        cout << networkAddress[i];
        if (i < 3) cout << ".";
    }
    cout << endl;
    cout << "Broadcast Address: ";
    for (int i = 0; i < 4; ++i) {
        cout << broadcastAddress[i];
        if (i < 3) cout << ".";
    }
    cout << endl;
    cout << "Valid IP Address Range: ";
    for (int i = 0; i < 4; ++i) {
        cout << validRange.first[i];
        if (i < 3) cout << ".";
    }
    cout << " - ";
    for (int i = 0; i < 4; ++i) {
        cout << validRange.second[i];
        if (i < 3) cout << ".";
    }
    cout << endl;
    return 0;
}

