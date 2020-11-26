#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/User.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/CFG.h"
#include <bits/stdc++.h> 
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include <fstream>
#include <stdint.h>
#include <set>
#include "llvm/Support/CommandLine.h"

using namespace llvm;

static cl::opt<bool> Branch_check("branch", cl::desc("Raise an error if the program branches on a private value"));

namespace {
  struct SensitivityAnalysis : public ModulePass 
  {
    static char ID; // Pass identification, replacement for typeid

    std::map<std::string, uint8_t> global_variables;
    std::map<std::string, uint8_t> functions;
    std::map<std::string, uint8_t> parameters;
    std::map<std::string, uint8_t> locals;
    std::map<std::string, std::unordered_set<Function*> > global_users;
    std::unordered_set<Function*> work_list;
    int max_live;

    SensitivityAnalysis() : ModulePass(ID) 
    {
      std::string line;
      std::ifstream infile("Metadata.csv");
      while (std::getline(infile, line))
      {
        std::vector<std::string> v;
        std::stringstream ss(line);
        while(ss.good()) 
        {
          std::string substr;
          std::getline(ss, substr, ',');
          v.push_back(substr);
        }
        if(v[0] == "Global")
        {
          if(v[2] == "1")
          {
            global_variables.insert(std::pair<std::string, uint8_t>(v[1], 1));
          }
          else
          {
            global_variables.insert(std::pair<std::string, uint8_t>(v[1], 0));
          }
        }
        else if(v[0] == "Function")
        {
          functions.insert(std::pair<std::string, uint8_t>(v[1], 0));
        }
        else if(v[0] == "Parameter")
        {
          if(v[4] == "1")
          {
            parameters.insert(std::pair<std::string,uint8_t>(v[1] + "::" + v[2], 1));
            locals.insert(std::pair<std::string, uint8_t>(v[1] + "::" + v[3], 1));
          }
          else
          {
            parameters.insert(std::pair<std::string,uint8_t>(v[1] + "::" + v[2], 0));
            locals.insert(std::pair<std::string, uint8_t>(v[1] + "::" + v[3], 0));
          }
        }
        else if(v[0] == "Local")
        {
          if(v[2] == "1")
          {
            locals.insert(std::pair<std::string, uint8_t>(v[1], 1));
          }
          else
          {
            locals.insert(std::pair<std::string, uint8_t>(v[1], 0));
          }
        }
      }

      std::cout << "Printing information about global variables:\n";
      for(auto iterator = global_variables.begin(); iterator != global_variables.end(); iterator++)
      {
        if(iterator->second == 1)
        {
          std::cout << iterator->first << " is a pointer\n";
        }
        else
        {
          std::cout << iterator->first << " is not a pointer\n";
        }
      }
      std::cout << "\n";

      std::cout << "Printing information about Local variables:\n";
      for(auto iterator = locals.begin(); iterator != locals.end(); iterator++)
      {
        if(iterator->second == 1)
        {
          std::cout << iterator->first << " is a pointer\n";
        }
        else
        {
          std::cout << iterator->first << " is not a pointer\n";
        }
      }
      std::cout << "\n";

      std::cout << "Printing information about Parameter variables:\n";
      for(auto iterator = parameters.begin(); iterator != parameters.end(); iterator++)
      {
        if(iterator->second == 1)
        {
          std::cout << iterator->first << " is a pointer\n";
        }
        else
        {
          std::cout << iterator->first << " is not a pointer\n";
        }
      }
      std::cout << "\n";

      std::cout << "Printing information about functions:\n";
      for(auto iterator = functions.begin(); iterator != functions.end(); iterator++)
      {
        if(iterator->second == 1)
        {
          std::cout << iterator->first << " is a pointer\n";
        }
        else
        {
          std::cout << iterator->first << " is not a pointer\n";
        }
      }
      std::cout << "\n";
    }

    bool run_analysis()
    {
      while(!work_list.empty())
      {
        Function* F = *(work_list.begin());
        bool changed = true;
        work_list.erase(work_list.begin());
        int iteration_number = 0;

        std::map<std::string, std::string> argument_list;
        int arg_number = 0;
        for(auto argument = F->arg_begin(); argument != F->arg_end(); argument++)
        {
          // outs() << argument->getName() << "\n";
          std::string arg_name = std::string(F->getName()) + "::" + std::string(argument->getName());
          std::string arg_new_name = std::string(F->getName()) + "::" + std::to_string(arg_number);
          argument_list.insert(std::pair<std::string, std::string>(arg_name, arg_new_name));
          arg_number++;
        }

        while(changed)
        {
          iteration_number++;
          changed = false;
          outs() << "\nITERATION: " << iteration_number << " on " << F->getName() <<"\n";
          for(inst_iterator I = inst_begin(*F); I != inst_end(*F); I++)
          {
            int global_modified = 0;
            int local_modified = 0;
            int is_dest_global = 0;
            std::string modified_local;
            std::string modified_global;

            if(I->getOpcode() == Instruction::Load)
            {
              std::string source = I->getOperand(0)->getName();
              std::string destination = I->getName();
              int is_private = 0;
              int is_source_global = 0;
              if(dyn_cast<GlobalVariable>(I->getOperand(0)))
              {
                is_source_global = 1;
                if(global_variables.find(source) != global_variables.end())
                {
                  is_private = 1;
                }
              }
              else
              {
                source = std::string(F->getName()) + "::" + source;
                if(locals.find(source) != locals.end())
                {
                  is_private = 1;
                }
              }

              if(is_private == 1)
              {
                int is_dest_pointer = 0;
                if(I->getType()->isPointerTy())
                {
                  is_dest_pointer = 1;
                }
                if(dyn_cast<GlobalVariable>(&*I))
                {
                  is_dest_global = 1;
                }
                if(is_dest_pointer == 1 && is_dest_global == 1)
                {
                  if(global_variables.find(destination) == global_variables.end())
                  {
                    global_variables.insert(std::pair<std::string, uint8_t>(destination, 1));
                    outs() << "Marking global variable " << destination << " a private pointer\n";
                    changed = true;
                    global_modified = 1;
                    modified_global = destination;
                  } 
                }
                else if(is_dest_pointer == 1 && is_dest_global == 0)
                {
                  destination = std::string(F->getName()) + "::" + destination;
                  if(locals.find(destination) == locals.end())
                  {
                    locals.insert(std::pair<std::string, uint8_t>(destination, 1));
                    outs() << "Marking local variable " << destination << " a private pointer\n";
                    changed = true;
                    local_modified = 1;
                    modified_local = destination;
                  }
                }
                else if(is_dest_pointer == 0 && is_dest_global == 1)
                {
                  if(global_variables.find(destination) == global_variables.end())
                  {
                    global_variables.insert(std::pair<std::string, uint8_t>(destination, 0));
                    outs() << "Marking global variable " << destination << " private\n";
                    changed = true;
                    global_modified = 1;
                    modified_global = destination;
                  }
                }
                else
                {
                  destination = std::string(F->getName()) + "::" + destination;
                  if(locals.find(destination) == locals.end())
                  {
                    locals.insert(std::pair<std::string, uint8_t>(destination, 1));
                    outs() << "Marking local variable " << destination << " private\n";
                    changed = true;
                    local_modified = 1;
                    modified_local = destination;
                  }
                }
              }
              /*If destination is private, mark source as private too*/
              int source_private = 0;
              if(is_dest_global == 0)
              {
                destination = std::string(F->getName()) + "::" + destination;
              }
              if(is_dest_global && global_variables.find(destination) != global_variables.end())
              {
                source_private = 1;
              }
              else if (locals.find(destination) != locals.end())
              {
                source_private = 1;
              }
              if(source_private)
              {
                int is_source_pointer = 0;
                if(I->getOperand(0)->getType()->isPointerTy())
                {
                  is_source_pointer = 1;
                }
                if(is_source_global == 1)
                {
                  if(global_variables.find(source) == global_variables.end())
                  {
                    global_variables.insert(std::pair<std::string, uint8_t>(source, is_source_pointer));
                    changed = true;
                    global_modified = 1;
                    modified_global = source;
                    outs() << "Marking global variable " << source << " as private. Ponter: " << is_source_pointer << "\n";
                  }
                }
                else
                {
                  if(locals.find(source) == locals.end())
                  {
                    locals.insert(std::pair<std::string, uint8_t>(source, is_source_pointer));
                    changed = true;
                    local_modified = 1;
                    modified_local = source;
                    outs() << "Marking local variable " << source << " as private. Ponter: " << is_source_pointer << "\n"; 
                  }
                }
              }
            }
            else if(I->getOpcode() == Instruction::Store)
            {
              std::string source = I->getOperand(0)->getName();
              std::string destination = I->getOperand(1)->getName();
              int is_dest_pointer = 0;
              int is_dest_global = 0;
              int is_source_global = 0;
              if(source == destination)
              {
                continue;
              }
              int is_private = 0;
              if(dyn_cast<GlobalVariable>(I->getOperand(0)))
              {
                is_source_global = 1;
                if(global_variables.find(source) != global_variables.end())
                {
                  is_private = 1;
                }
              }
              else
              {
                source = std::string(F->getName()) + "::" + source;
                if(locals.find(source) != locals.end())
                {
                  is_private = 1;
                }
              }

              if(is_private == 1) /*Marking destination as sensitive*/
              {
                if(I->getOperand(1)->getType()->isPointerTy())
                {
                  is_dest_pointer = 1;
                }
                if(dyn_cast<GlobalVariable>(&*(I->getOperand(1))))
                {
                  is_dest_global = 1;
                }
                if(is_dest_pointer == 1 && is_dest_global == 1)
                {
                  if(global_variables.find(destination) == global_variables.end())
                  {
                    global_variables.insert(std::pair<std::string, uint8_t>(destination, 1));
                    outs() << "Marking global variable " << destination << " a private pointer\n";
                    changed = true;
                    global_modified = 1;
                    modified_global = destination;
                  }
                }
                else if(is_dest_pointer == 1 && is_dest_global == 0)
                {
                  destination = std::string(F->getName()) + "::" + destination;
                  if(locals.find(destination) == locals.end())
                  {
                    locals.insert(std::pair<std::string, uint8_t>(destination, 1));
                    outs() << "Marking local variable " << destination << " a private pointer\n";
                    changed = true;
                    local_modified = 1;
                    modified_local = destination;
                  }
                }
                else if(is_dest_pointer == 0 && is_dest_global == 1)
                {
                  if(global_variables.find(destination) == global_variables.end())
                  {
                    global_variables.insert(std::pair<std::string, uint8_t>(destination, 0));
                    outs() << "Marking global variable " << destination << " private\n";
                    changed = true;
                    global_modified = 1;
                    modified_global = destination;
                  }
                }
                else
                {
                  destination = std::string(F->getName()) + "::" + destination;
                  if(locals.find(destination) == locals.end())
                  {
                    locals.insert(std::pair<std::string, uint8_t>(destination, 1));
                    outs() << "Marking local variable " << destination << " private\n";
                    changed = true;
                    local_modified = 1;
                    modified_local = destination;
                  }
                }
              }

              /*If destination is private, mark source as private too*/
              int source_private = 0;
              if(is_dest_global == 0)
              {
                destination = std::string(F->getName()) + "::" + destination;
              }
              if(is_dest_global && global_variables.find(destination) != global_variables.end())
              {
                source_private = 1;
              }
              else if (locals.find(destination) != locals.end())
              {
                source_private = 1;
              }
              if(source_private == 1)
              {
                int is_source_pointer = 0;
                if(I->getOperand(0)->getType()->isPointerTy())
                {
                  is_source_pointer = 1;
                }
                if(is_source_global == 1)
                {
                  if(global_variables.find(source) == global_variables.end())
                  {
                    global_variables.insert(std::pair<std::string, uint8_t>(source, is_source_pointer));
                    changed = true;
                    global_modified = 1;
                    modified_global = source;
                    outs() << "Marking global variable " << source << " as private. Ponter: " << is_source_pointer << "\n";
                  }
                }
                else
                {
                  if(locals.find(source) == locals.end())
                  {
                    locals.insert(std::pair<std::string, uint8_t>(source, is_source_pointer));
                    changed = true;
                    local_modified = 1;
                    modified_local = source;
                    outs() << "Marking local variable " << source << " as private. Ponter: " << is_source_pointer << "\n"; 
                  }
                }
              }
            }
            else if(I->getOpcode() == Instruction::Br)
            {
              auto branch_inst = dyn_cast<BranchInst>(&*I);
              if(Branch_check && branch_inst->isConditional())
              {
                auto branch_variable = I->getOperand(0);
                int is_private = 0;
                if(dyn_cast<GlobalVariable>(branch_variable))
                {
                  if(global_variables.find(std::string(branch_variable->getName())) != global_variables.end())
                  {
                    is_private = 1;
                  }
                }
                else
                {
                  if(locals.find(std::string(F->getName()) + "::" + std::string(branch_variable->getName())) != locals.end())
                  {
                    is_private = 1;
                  }
                }
                if(is_private == 1)
                {
                  outs() << "\nERROR: Branching on private value " << branch_variable->getName() << "\n";
                  return true;
                }
              }
            }
            else if(I->getOpcode() == Instruction::Ret)
            {
              int is_func_private = 0;
              int is_retval_private = 0;
              if(functions.find(std::string(F->getName())) != functions.end())
              {
                is_func_private = 1;
              }
              std::string retval = I->getOperand(0)->getName();
              if(dyn_cast<GlobalVariable>(I->getOperand(0)))
              {
                if(global_variables.find(retval) != global_variables.end())
                {
                  is_retval_private = 1;
                }
              }
              else
              {
                retval = std::string(F->getName()) + "::" + retval;
                if(locals.find(retval) != locals.end())
                {
                  is_retval_private = 1;
                }
              }
              if(is_func_private == 0 && is_retval_private == 1)
              {
                outs() << "\nERROR: Function " << F->getName() << " marked public, but is returning a private value!\n";
                return true;
              }
            }
            else if(I->getOpcode() == Instruction::Call)
            {
              auto call_instruction = dyn_cast<CallInst>(&*I);
              int num_operands = call_instruction->getNumOperands();
              std::string func_name = call_instruction->getOperand(num_operands-1)->getName();
              for(int i = 0; i < num_operands-1; i++)
              {
                auto call_parameter = call_instruction->getOperand(i);
                auto caller_name = func_name + "::" + std::to_string(i);
                if(call_parameter->hasName())
                {
                  if(parameters.find(caller_name) != parameters.end()) /*Parameter is private, propagate taint*/
                  {
                    int is_pointer = 0;
                    if(call_parameter->getType()->isPointerTy())
                    {
                      is_pointer = 1;
                    }
                    if(dyn_cast<GlobalVariable>(call_parameter))
                    {
                      if(global_variables.find(call_parameter->getName()) == global_variables.end())
                      {
                        global_modified = 1;
                        modified_global = call_parameter->getName();
                        global_variables.insert(std::pair<std::string, uint8_t>(call_parameter->getName(), is_pointer));
                        changed = true;
                        outs() << "Marking " << call_parameter->getName() << " as private. Pointer: " << is_pointer << "\n";
                      }
                    }
                    else
                    {
                      std::string var_name = std::string(F->getName()) + "::" + std::string(call_parameter->getName());
                      if(locals.find(var_name) == locals.end())
                      {
                        local_modified = 1;
                        modified_local = var_name;
                        locals.insert(std::pair<std::string, uint8_t>(var_name, is_pointer));
                        changed = true;
                        outs() << "Marking " << var_name << " as private. Pointer: " << is_pointer << "\n";
                      }
                    }
                  }
                  else /*Parameter is public, a private value should not be passed*/
                  {
                    int is_private = 0;
                    if(dyn_cast<GlobalVariable>(call_parameter))
                    {
                      if(global_variables.find(call_parameter->getName()) != global_variables.end())
                      {
                        is_private = 1;
                      }
                    }
                    else
                    {
                      if(locals.find(std::string(F->getName()) + "::" + std::string(call_parameter->getName())) != locals.end())
                      {
                        is_private = 1;
                      }
                    }
                    if(is_private == 1)
                    {
                      outs() << "\nPassing a private value, " << call_parameter->getName() << ", to a public parameter!\n";
                      return true;  
                    }
                  }
                }
              }
              if(functions.find(func_name) != functions.end()) /*Function returns a private value*/
              {
                std::string destination = call_instruction->getName();
                int is_global = 0;
                if(dyn_cast<GlobalVariable>(call_instruction))
                {
                  is_global = 1;
                }
                else
                {
                  destination = std::string(F->getName()) + "::" + destination;
                }
                int is_pointer = 0;
                if(call_instruction->getType()->isPointerTy())
                {
                  is_pointer = 1;
                }
                if(is_global == 1)
                {
                  if(global_variables.find(destination) == global_variables.end())
                  {
                    changed = true;
                    global_variables.insert(std::pair<std::string, uint8_t>(destination, is_pointer));
                    global_modified = 1;
                    modified_global = destination;
                    outs() << "Marking global variable " << destination << " as private. Pointer: " << is_pointer << "\n";
                  }
                }
                else
                {
                  if(locals.find(destination) == locals.end())
                  {
                    changed = true;
                    locals.insert(std::pair<std::string, uint8_t>(destination, is_pointer));
                    local_modified = 1;
                    modified_local = destination;
                    outs() << "Marking local variable " << destination << " as private. Pointer: " << is_pointer << "\n"; 
                  }
                }
              }
            }
            else
            {
              int is_private = 0;
              for (auto operand_it = I->op_begin(); operand_it != I->op_end(); ++operand_it)
              {
                std::string operand = (*operand_it)->getName();
                if(operand.length() > 0 && !(operand.rfind("bb", 0) == 0))
                {
                  if(dyn_cast<GlobalVariable>(&*operand_it))
                  {
                    if(global_variables.find(operand) != global_variables.end())
                    {
                      is_private = 1;
                      break;
                    }
                  }
                  else
                  {
                    if(locals.find(std::string(F->getName()) + "::" + operand) != locals.end())
                    {
                      is_private = 1;
                    }
                  }
                }
              }
              if(is_private == 1)
              {
                std::string destination = I->getName();
                int is_dest_pointer = 0;
                if(I->getType()->isPointerTy())
                {
                  is_dest_pointer = 1;
                }
                int is_dest_global = 0;
                if(dyn_cast<GlobalVariable>(&*I))
                {
                  is_dest_global = 1;
                }
                if(is_dest_pointer == 1 && is_dest_global == 1)
                {
                  if(global_variables.find(destination) == global_variables.end())
                  {
                    global_variables.insert(std::pair<std::string, uint8_t>(destination, 1));
                    outs() << "Marking global variable " << destination << " a private pointer\n";
                    changed = true;
                    global_modified = 1;
                    modified_global = destination;
                  }
                }
                else if(is_dest_pointer == 1 && is_dest_global == 0)
                {
                  destination = std::string(F->getName()) + "::" + destination;
                  if(locals.find(destination) == locals.end())
                  {
                    locals.insert(std::pair<std::string, uint8_t>(destination, 1));
                    outs() << "Marking local variable " << destination << " a private pointer\n";
                    changed = true;
                    local_modified = 1;
                    modified_local = destination;
                  }
                }
                else if(is_dest_pointer == 0 && is_dest_global == 1)
                {
                  if(global_variables.find(destination) == global_variables.end())
                  {
                    global_variables.insert(std::pair<std::string, uint8_t>(destination, 0));
                    outs() << "Marking global variable " << destination << " private\n";
                    changed = true;
                    global_modified = 1;
                    modified_global = destination;
                  }
                }
                else
                {
                  destination = std::string(F->getName()) + "::" + destination;
                  if(locals.find(destination) == locals.end())
                  {
                    locals.insert(std::pair<std::string, uint8_t>(destination, 1));
                    outs() << "Marking local variable " << destination << " private\n";
                    changed = true;
                    local_modified = 1;
                    modified_local = destination;
                  }
                }
              }
            }
            /*If a global variable is modified add the functions using it back to worklist*/
            if(global_modified == 1)
            {
              auto user_functions = global_users[modified_global];
              for(auto user = user_functions.begin(); user != user_functions.end(); user++)
              {
                outs() << "Adding " << (*user)->getName() << " to worklist\n";
                work_list.insert(*user);
              }
            }
            /*If a parameter not marked as private is inferred to be private, raise an error*/
            if(local_modified == 1 && argument_list.find(modified_local) != argument_list.end())
            {
              std::string parameter_name = argument_list[modified_local];
              if(parameters.find(parameter_name) == parameters.end())
              {
                outs() << "\nERROR: Parameter " << modified_local << " marked as public, but inferred to be private!\n";
                return true;
              }
            }
          }
        }
      }
      return false;
    }

    bool runOnModule(Module &M) override 
    {
      for (auto F = M.getFunctionList().begin(), end = M.getFunctionList().end(); F != end; ++F)
      {
        outs() << "Found function " << F->getName() << "\n";
        work_list.insert(&*F);

        for (inst_iterator I = inst_begin(*F), E = inst_end(*F); I != E; ++I)
        {
          for (auto operand_it = I->op_begin(); operand_it != I->op_end(); ++operand_it)
          {
            if(dyn_cast<GlobalVariable>(*operand_it))
            {
              std::string operand = (*operand_it)->getName();
              outs() << "Function " << F->getName() << " uses global variable " << operand << "\n";
              auto user_set = global_users[operand];
              user_set.insert(&*F);
              global_users[operand] = user_set;
            }
          }
        }
      }

      outs() << "\nFollowing functions are in work_list:\n";
      for(auto iterator = work_list.begin(); iterator != work_list.end(); iterator++)
      {
        outs() << (*iterator)->getName() << "\n";
      }
      outs() << "\n";

      outs() << "Global variable users:\n";
      for(auto iterator = global_users.begin(); iterator != global_users.end(); iterator++)
      {
        outs() << iterator->first << " : ";
        for(auto it = (iterator->second).begin(); it != (iterator->second).end(); it++)
        {
          outs() << (*it)->getName() << " ";
        }
        outs() << "\n";
      }
      outs() << "\n";

      bool error = run_analysis();
      if(error)
      {
        outs() << "Please correct the errors, sensitive data may leak!\n"; 
      }
      else
      {
        outs() << "No errors detected!\n";
      }

      return false;
    }
  };
}

char SensitivityAnalysis::ID = 0;
static RegisterPass<SensitivityAnalysis> X("sensitive_analysis", "Sensitivity Analysis Pass");