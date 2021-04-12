#include "CommandLineParsing.h"
#include <iostream>

CommandLineParsing::CommandLineParsing(int& argc, char**& argv) :
    m_msec(0),
    m_msecSet(false),
    m_debugSet(false)
{
    if (argc <= 0 || argv == nullptr)
        return;

    // Creating a cxxopts option object to parsing the command line arguments.
    cxxopts::Options options("KeyChattering", "Eleminate key chattering on mechanical keyboard");

    // Adding the command line options.
    options.add_options()
        ("t,time", "Time since last press of the same key to treat this key has a chatter", cxxopts::value<int>())
        ("d,debug", "Print debug information when a key is chattering")
        ("h,help", "Print usage information.");

    // Parsing the command line.
    cxxopts::ParseResult result = options.parse(argc, argv);

    // If the help options is in the command line argument,
    // print the help indication and exit.
    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        std::exit(EXIT_SUCCESS);
    }

    // Retrieve time options.
    if (result.count("time"))
    {
        try
        {
            m_msec = result["time"].as<int>();
            m_msecSet = true;
        }
        catch (const cxxopts::OptionParseException& e)
        {
            std::cerr << "-t,--time, invalid argument. The argument must be a positive number." << std::endl;
#ifndef NDEBUG
            std::cerr << e.what() << std::endl;
#endif
            std::exit(EXIT_FAILURE);
        }

        if (m_msec <= 0)
        {
            std::cerr << "-t, --time, invalid argument. The argument must be a positive number." << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    // Check if debug is set.
    if (result.count("debug"))
        m_debugSet = true;
}

bool CommandLineParsing::isMSecSet() const
{
    return m_msecSet;
}

int CommandLineParsing::msec() const
{
    return m_msec;
}

bool CommandLineParsing::isDebugSet() const
{
    return m_debugSet;
}