/*
 * CommandHandler.h
 *
 *  Created on: Jan 2, 2015
 *      Author: dwarf
 */

#ifndef GUI_COMMANDHANDLER_H_
#define GUI_COMMANDHANDLER_H_

#include <string>

namespace gui{
std::string handleCommand(std::string);

std::string autocomplete(std::string);
std::string getPossibleCommands(std::string);

}



#endif /* GUI_COMMANDHANDLER_H_ */
