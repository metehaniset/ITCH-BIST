#include <Reader.hpp>

Reader::Reader(const std::string &_fileName, const std::string &_stock):
    fileName(_fileName), stock(_stock){
        file.open(fileName);
        if(!file.is_open()){
            std::cerr << "The input file: " << fileName << " cannot be open! " << std::endl;
            }
        else{
            std::cout << "Opened " << fileName << " to read ITCH 5.0. messages." << std::endl;
            validFile = 1;
        }
        start = time(0);
    }

Reader::Reader(const std::string &_stock): stock(_stock){}

void Reader::printProgress(void){
    count ++;
    if(count % 10000000 == 0){
        std::cout << "Processed " << count/1000000 << "Mio messages. " << int(count/difftime(time(0), start)/10000)/100. << " Mio messages per sec." << std::endl;
    }
}

void Reader::readBytesIntoMessage(const long &size){
    file.read(message, size);
}

void Reader::skipBytes(const long &size){
    file.ignore(size);
}

char Reader::getKey(void){
    char key;
    file.get(key);
    return key;
}

Message Reader::createMessage(void){
    printProgress();
    Message msg;
    skipBytes(2);
    char key = getKey();
    uint32_t msgBookId = orderBookId;
//    char ticker[33];
//    strncpy(ticker, stock.c_str(), 32); ticker[32] = 0;
//    std::cout << "Key: " << key << std::endl;
    switch(key){
        uint64_t timeStamp, orderId, oldOrderId, newOrderId, size, execSize, newSize;
        uint32_t price, newPrice;
        // int16_t = numberOfDecimals;
        char direction, ticker[33];
        char mpid[8];

        case 'T':	// Timestamp
        	readBytesIntoMessage(4);
        	lastTimestamp = parse_uint32(message);
        	// std::cout << "New timestamp " << lastTimestamp << std::endl;
        	break;
        case 'R':	// OrderBookDirectory
            readBytesIntoMessage(128);
            // strncpy(ticker, message+8, 32); ticker[32] = 0;
            // std::cout << "ticker " << ticker << std::endl;
            if (orderBookId == 0) {	 // ilk kez geliyor
            	strncpy(ticker, message+8, 32); ticker[32] = 0;
                if(strcmp(ticker,stock.c_str()) == 0){
                	// std::cout << ticker << stock.c_str() << std::endl;
                	orderBookId = parse_uint32(message+4);	// ilgilendiğimiz order book budur
                	numberOfDecimals = parse_uint16(message+88);
                	// numberOfDecimals = pow(10, x);
                	std::cout << "orderBookId found: " << ticker <<  orderBookId  << " " << numberOfDecimals << std::endl;
                }
            };
            // std::cout << "orderBookId: " << " " << orderBookId << std::endl;
            break;
        case 'M':	// Combination Order Book Leg
            readBytesIntoMessage(17);
            break;
        case 'L':	// Tick Size Table Entry
            readBytesIntoMessage(24);
            break;
        case 'S':	// System Message
            readBytesIntoMessage(5);
//            char event = message[4];
//            if (event == 'O') {
//            	std::cout << "Day is starting" << std::endl;
//            }
//            else {
//            	std::cout << "Day is ending" << std::endl;
//            };
            break;
        case 'O':	// Orderbook state message
        	readBytesIntoMessage(28);
        	break;
        case 'A':
            readBytesIntoMessage(36);
            // timeStamp = parse_uint32(message);
//            uint64_t x;
//            x = (static_cast<uint64_t>(lastTimestamp) << 32);
//            uint32_t y;
//            y = parse_uint32(message);
            timeStamp = parse_uint32(message);
            //timeStamp = (static_cast<uint64_t>(lastTimestamp) << 32)  + parse_uint32(message);
            // std::cout << lastTimestamp << " " << x << " " << y << " " << timeStamp << std::endl;
            orderId = parse_uint64(message+4);
            direction = message[16];
            size = parse_uint64(message+21);
            msgBookId = parse_uint32(message+12);
            price = parse_uint32(message+29);
            // if(debug){
            //    locateCode = parse_uint16(message);
            //    trackingNumb = parse_uint16(message+2);
            //     sprintf(str,"%c,%" PRIu16 ",%" PRIu16 ",%" PRIu64 ",%" PRIu64 ",%c,%" PRIu32 ",%s,%" PRIu32 ".%04" PRIu32 "\n",
            //         key,locateCode,trackingNumb,timeStamp,orderId,
            //         direction,size,ticker,
            //         price/10000,price%10000);
            // }
            msg.setType(key);
            msg.setTimeStamp_s(static_cast<time_type>(lastTimestamp));
            msg.setTimeStamp_ns(static_cast<time_type>(timeStamp));
            msg.setId(static_cast<id_type>(orderId));
            msg.setSide(static_cast<side_type>(direction == 'S'));
            msg.setRemSize(static_cast<size_type>(size));
            msg.setPrice(static_cast<price_type>(price)/1000);
            // std::cout << key << " " << timeStamp << " " << direction << " " << size << " " << price << " " << msgBookId << " " << orderId << std::endl;
            break;
        case 'F':
            readBytesIntoMessage(43);
            timeStamp = parse_uint32(message);
            //timeStamp = (((unsigned long long)lastTimestamp) << 32)  + parse_uint32(message);
            orderId = parse_uint64(message+4);
            direction = message[16];
            size = parse_uint64(message+21);
            msgBookId = parse_uint32(message+12);
            price = parse_uint32(message+29);
            // if(debug){
            //    locateCode = parse_uint16(message);
            //    trackingNumb = parse_uint16(message+2);
            strncpy(mpid, message+36, 7); mpid[7] = 0;
            //     sprintf(str,"%c,%" PRIu16 ",%" PRIu16 ",%" PRIu64 ",%" PRIu64 ",%c,%" PRIu32 ",%s,%" PRIu32 ".%04" PRIu32 ",%s\n",
            //     key,locateCode,trackingNumb,timeStamp,orderId,
            //     direction,size,ticker,
            //     price/10000,price%10000,mpid);
            // }
            msg.setType(key);
            msg.setTimeStamp_s(static_cast<time_type>(lastTimestamp));
            msg.setTimeStamp_ns(static_cast<time_type>(timeStamp));
            msg.setId(static_cast<id_type>(orderId));
            msg.setSide(static_cast<side_type>(direction == 'S'));
            msg.setRemSize(static_cast<size_type>(size));
            msg.setPrice(static_cast<price_type>(price)/1000);
            msg.setMPID(*mpid);
            break;

        case 'E':
            readBytesIntoMessage(51);
            // timeStamp = parse_ts(message+4);
            timeStamp = parse_uint32(message);
            //timeStamp = (((unsigned long long)lastTimestamp) << 32)  + parse_uint32(message);
            orderId = parse_uint64(message+4);
            msgBookId = parse_uint32(message+12);
            execSize = parse_uint64(message+17);
            // if(debug){
            //    locateCode = parse_uint16(message);
            //    trackingNumb = parse_uint16(message+2);
            //     matchNumber = parse_uint64(message+22);
            //     sprintf(str,"%c,%" PRIu16 ",%" PRIu16 ",%" PRIu64 ",%" PRIu64 ",%" PRIu32 ",%" PRIu64 "\n",
            //     key,locateCode,trackingNumb,timeStamp,orderId,
            //     execSize,matchNumber);
            // }
            msg.setType(key);
            msg.setTimeStamp_s(static_cast<time_type>(lastTimestamp));
            msg.setTimeStamp_ns(static_cast<time_type>(timeStamp));
            msg.setId(static_cast<id_type>(orderId));
            msg.setExecSize(static_cast<size_type>(execSize));
            break;

        case 'C':
            readBytesIntoMessage(57);
            // timeStamp = parse_ts(message);
            timeStamp = parse_uint32(message);
            //timeStamp = (((unsigned long long)lastTimestamp) << 32)  + parse_uint32(message);
            orderId = parse_uint64(message+4);
            execSize = parse_uint64(message+17);
            msgBookId = parse_uint32(message+12);
            price = parse_uint32(message+51);
            // if(debug){
            //    locateCode = parse_uint16(message);
            //    trackingNumb = parse_uint16(message+2);
            //     matchNumber = parse_uint64(message+22);
            //     printable = message[30];
            //     sprintf(str,"%c,%" PRIu16 ",%" PRIu16 ",%" PRIu64 ",%" PRIu64 ",%" PRIu32 ",%" PRIu64 ",%c,%" PRIu32 ".%04" PRIu32 "\n",
            //     key,locateCode,trackingNumb,timeStamp,orderId,
            //     execSize,matchNumber,printable,
            //     price/10000,price%10000);
            // }
            msg.setType(key);
            msg.setTimeStamp_s(static_cast<time_type>(lastTimestamp));
            msg.setTimeStamp_ns(static_cast<time_type>(timeStamp));
            msg.setId(static_cast<id_type>(orderId));
            msg.setExecSize(static_cast<size_type>(execSize));
            msg.setPrice(static_cast<price_type>(price)/1000);
            break;
        case 'U':
            readBytesIntoMessage(35);
            timeStamp = parse_uint32(message);
            //timeStamp = (((unsigned long long)lastTimestamp) << 32)  + parse_uint32(message);
            oldOrderId = parse_uint64(message+4);
            newOrderId = parse_uint64(message+4);
            newSize = parse_uint64(message+21);
            newPrice = parse_uint32(message+29);
            msgBookId = parse_uint32(message+12);
            // if(debug){
            //    locateCode = parse_uint16(message);
            //    trackingNumb = parse_uint16(message+2);
            //     sprintf(str,"%c,%" PRIu16 ",%" PRIu16 ",%" PRIu64 ",%" PRIu64 ",%" PRIu64 ",%" PRIu32 ",%" PRIu32 ".%04" PRIu32 "\n",
            //     key,locateCode,trackingNumb,timeStamp,oldOrderId,
            //     newOrderId,newSize,newPrice/10000,newPrice%10000);
            // }
            msg.setType(key);
            msg.setTimeStamp_s(static_cast<time_type>(lastTimestamp));
            msg.setTimeStamp_ns(static_cast<time_type>(timeStamp));
            msg.setId(static_cast<id_type>(newOrderId));
            msg.setOldId(static_cast<id_type>(oldOrderId));
            msg.setRemSize(static_cast<size_type>(newSize));
            msg.setPrice(static_cast<price_type>(newPrice)/1000);
            break;

        case 'D':
            readBytesIntoMessage(17);
            timeStamp = parse_uint32(message);
            // timeStamp = (((unsigned long long)lastTimestamp) << 32)  + parse_uint32(message);
            orderId = parse_uint64(message+4);
            msgBookId = parse_uint32(message+12);
            // if(debug){
            //    locateCode = parse_uint16(message);
            //    trackingNumb = parse_uint16(message+2);
            //     sprintf(str,"%c,%" PRIu16 ",%" PRIu16 ",%" PRIu64 ",%" PRIu64 "\n",
            //     key,locateCode,trackingNumb,timeStamp,orderId);
            // }
            msg.setType(key);
            msg.setTimeStamp_s(static_cast<time_type>(lastTimestamp));
            msg.setTimeStamp_ns(static_cast<time_type>(timeStamp));
            msg.setId(static_cast<id_type>(orderId));
            break;
        case 'P':
            readBytesIntoMessage(49);
            timeStamp = parse_uint32(message);
            //timeStamp = (((unsigned long long)lastTimestamp) << 32)  + parse_uint32(message);
            orderId = parse_uint64(message+4);
            direction = message[16];
            size = parse_uint64(message+17);
            // strncpy(ticker, message+23, 8); ticker[8] = 0;
            msgBookId = parse_uint32(message+25);
            price = parse_uint32(message+29);
            // if(debug){
            //    locateCode = parse_uint16(message);
            //    trackingNumb = parse_uint16(message+2);
            //    matchId = parse_uint64(message+35);
            //     sprintf(str,"%c,%" PRIu16 ",%" PRIu16 ",%" PRIu64 ",%" PRIu64 ",%c,%" PRIu32 ",%s,%" PRIu32 ".%04" PRIu32 ",%" PRIu64 "\n",
            //     key,locateCode,trackingNumb,timeStamp,orderId,
            //     direction,size,ticker,price/10000,price%10000,matchId);
            // }
            msg.setType(key);
            msg.setTimeStamp_s(static_cast<time_type>(lastTimestamp));
            msg.setTimeStamp_ns(static_cast<time_type>(timeStamp));
            msg.setId(static_cast<id_type>(orderId));
            msg.setSide(static_cast<side_type>(direction == 'S'));
            msg.setExecSize(static_cast<size_type>(size));
            msg.setPrice(static_cast<price_type>(price)/1000);

            break;
        case 'Z':	// Equilibrium Price Update
        	 readBytesIntoMessage(52);
        	 break;

        default:
            if(!eof()){
                std::cerr << "Type " << key <<" not found: abort" << std::endl;
                validFile = 0;
            }
            break;
    }
    // if(debug){
    //     parserWriter.writeLine(std::string(str));
    // }
    if(msgBookId != orderBookId){
        return Message();
    }
    return msg;
}

bool Reader::eof(void){
    return file.eof();
}

bool Reader::isValid(void) const{
    return validFile;
}

std::string Reader::getFileName(void) const{
    return fileName;
}

std::string Reader::getStock(void) const{
    return stock;
}

void Reader::setMessage(const char* str){
    memcpy(message, str, 64);
}

Reader::~Reader(){
    if (file.is_open()){
        file.close();
        std::cout << "File " << fileName << " has been closed" << std::endl;
        std::cout << "Finished, processed " << count << " messages in " << difftime(time(0),start) << "seconds."  << std::endl;
    }
}
