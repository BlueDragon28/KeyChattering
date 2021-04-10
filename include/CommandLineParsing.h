#ifndef KEYCHATTERING_COMMANDLINEPARSING_H_
#define KEYCHATTERING_COMMANDLINEPARSING_H_

#include "cxxopts.hpp"

class CommandLineParsing
{
public:
    CommandLineParsing(int& argc, char**& argv);

    bool isMSecSet() const;
    int msec() const;

    bool isDebugSet() const;

private:
    bool m_msecSet;
    int m_msec;
    bool m_debugSet;
};

#endif // KEYCHATTERING_COMMANDLINEPARSING_H_