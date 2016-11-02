#ifndef IO_H
#define IO_H

// Based on an answer on StackOverflow, by James Kanze
// http://stackoverflow.com/a/24861313/63991

class PositionStreambuf: public std::streambuf {
public:
    PositionStreambuf(std::streambuf* source):
            mySource(source), myOwner(nullptr), myIsAtStartOfLine(true),
            line_no(0), col_no(0) { }

    PositionStreambuf(std::istream& owner):
            mySource(owner.rdbuf()), myOwner(&owner), myIsAtStartOfLine(true),
            line_no(0), col_no(0)
    {
        myOwner->rdbuf( this );
    }

    ~PositionStreambuf() {
        if (myOwner != nullptr) {
            myOwner->rdbuf(mySource);
        }
    }

    int line() const {
        return line_no;
    }

    int column() const {
        return col_no;
    }

protected:
    int underflow()
    {
        int ch = mySource->sbumpc();
        if (ch != EOF) {
            myBuffer = ch;
            setg(&myBuffer, &myBuffer, &myBuffer + 1);
            if (myIsAtStartOfLine) {
                line_no++;
                col_no = 0;
            }
            col_no++;
            myIsAtStartOfLine = myBuffer == '\n';
        }
        return ch;
    }
private:
    std::streambuf* mySource;
    std::istream* myOwner;
    bool myIsAtStartOfLine;
    char myBuffer;
    int line_no, col_no;
};

#endif
