/*** REMOVED FOR STYLE GUIDE VIOLATION ***/
if (ch == '0') { 
        ch = '1';
} else if (ch == '1') {
        ch = '0';
}

/*** REMOVED FOR STYLE GUIDE VIOLATION ***/


/*** REMOVED FOR STYLE GUIDE VIOLATION ***/
if (((periodPosition[i+1] - periodPosition[i] - 1) == 0) or (periodPosition[i]+1 == input.size())){
        valid = 0;
        return valid;
}

/*** REMOVED FOR STYLE GUIDE VIOLATION ***/
substr = binaryString.substr(8*i, 8);
bitset<32> binaryBits(substr);
values[i] = binaryBits.to_ulong();

/*** REMOVED FOR STYLE GUIDE VIOLATION ***/
if (input[i] == '.') {
        periodCounter++;
        if (periodCounter > 3) {
                valid = 0;
                return valid;
        }
        // save the period positions
        periodPosition[periodCounter] = i;
}

/*** REMOVED FOR STYLE GUIDE VIOLATION ***/
if (i == 3) {
        value = stoi(input.substr(periodPosition[i]+1));
}
else {
        value = stoi(input.substr(periodPosition[i]+1, (periodPosition[i+1] - periodPosition[i] - 1)));
}

/*** REMOVED FOR STYLE GUIDE VIOLATION ***/
if ((value > 255) and (value < 0)) {
        valid = 0;
        return valid;
}

/*** REMOVED FOR STYLE GUIDE VIOLATION ***/
if (input[i] == '.') {
        periodCounter++;
        // save the period positions
        periodPosition[periodCounter] = i;
}

