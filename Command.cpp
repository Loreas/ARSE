//
// Created by jonathan on 01.06.17.
//

#include "Command.h"
#include "Parser.h"


Command::Command(std::string name, std::string desc, std::string cmd, std::string regex,
                 std::string lang, std::string exec, std::string end, bool upToDate) {
    this->name = name;
    this->desc = desc;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

    command_regex = cmd;
    arg_regex = regex;
    language = lang;
    execute = exec;
    end_message = "Command '" + cmd + "' finished.\n"; // TODO: implement this?
    buildDFA(upToDate);
}

Command::~Command() {
    delete this->arg_dfa;
}

DFA* Command::getDFA() const {
    return arg_dfa;
}

std::string Command::getName() const {
    return name;
}

std::string Command::getCommand() const {
    return command_regex;
}

std::string Command::getDesc() const {
    return desc;
}

std::string Command::getArgRegex() const {
    return arg_regex;
}

std::string Command::getExecute() const {
    return execute;
}

std::string Command::getEndMessage() const {
    return end_message;
}

std::string Command::getLanguage() const {
    return language;
}

void Command::buildDFA(bool upToDate, bool output) {

    std::string filename = "./.config/command_" + getName();
    if(!upToDate) {
        // Make ENFA out of the regex
        ENFA enfa;
        RegToeNFA converter;
        std::string regex = getArgRegex();
        converter.ConvertReTo_eNfa(regex, enfa);
        if (output) enfa.FAtoDot((getName() + "_enfa"));

        // Use MSSC to make DFA
        DFA dfa_raw;
        MSSC(enfa, dfa_raw);
        if (output) dfa_raw.FAtoDot((getName() + "_dfaRaw"));

        // Use TFA on DFA
        DFA *dfa = new DFA;
        tfa(dfa_raw, dfa);
        if (output) dfa->FAtoDot((getName() + "_dfa"));
        // Write DFA to json
        dfa->FAtoJSON(filename);

        // Set dfa
        this->arg_dfa = dfa;
    }
    else{
        // Read DFA from json in .config
        Parser parser;
        DFA* dfa = new DFA;
        *dfa = parser.parseDFA(filename);
        this->arg_dfa = dfa;
    }
}