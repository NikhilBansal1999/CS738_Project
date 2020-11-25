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
    
    // std::map<BasicBlock*, std::unordered_set<std::string>*> block_kill;
    // std::map<BasicBlock*, std::unordered_set<std::string>*> block_gen;
    // std::map<BasicBlock*, std::unordered_set<std::string>*> block_in;
    // std::map<BasicBlock*, std::unordered_set<std::string>*> block_out;
    // std::map<int, int> num_live_points;

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

    // std::unordered_set<std::string>* all_variables(Function &F)
    // {
    // 	auto result = new std::unordered_set<std::string>;
    // 	for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I)
    // 	{
    // 		// outs() << *I << "\n";
	   //  	for (auto operand_it = I->op_begin(); operand_it != I->op_end(); ++operand_it)
	   //  	{
	   //  		std::string operand = (*operand_it)->getName();
	   //  		if(operand.length() > 0 && !(operand.rfind("bb", 0) == 0))
	   //  		{
	   //  			result->insert(operand);
	   //  			// outs() << "OPERAND: " << operand <<"\n";
	   //  		}
	   //  	}
    // 		std::string destination = I->getName();
    // 		if(destination.length() > 0)
    // 		{
	   //  		result->insert(destination);
	   //  		// outs() << "DESTINATION: " << destination <<"\n";
	   //  	}
    // 		// outs() << "\n"; 
    // 	}
    // 	return result;
    // }

    // void find_gen_fill(BasicBlock* bb)
    // {
    // 	std::unordered_set<std::string>* gen = new std::unordered_set<std::string>;
    // 	std::unordered_set<std::string>* kill = new std::unordered_set<std::string>;
    // 	for(Instruction &I : *bb)
    // 	{
    		// if(I.getOpcode() == Instruction::PHI) /*Handle phi instructions separately*/
    // 		{
    // 			std::string destination = I.getName();
    // 			if(destination.length() > 0 && gen->find(destination) == gen->end())
    // 			{
    // 				kill->insert(destination);
    // 			}
    // 		}
    // 		else
    // 		{
    // 			for (auto operand_it = I.op_begin(); operand_it != I.op_end(); ++operand_it)
		  //   	{
		  //   		std::string operand = (*operand_it)->getName();
		  //   		if(operand.length() > 0 && !(operand.rfind("bb", 0) == 0))
		  //   		{
		  //   			if(kill->find(operand) == kill->end()) /*Not yet defined*/
		  //   			{
		  //   				gen->insert(operand);
		  //   			}
		  //   		}
		  //   	}
		  //   	std::string destination = I.getName();
		  //   	if(I.getOpcode() != Instruction::Store)
		  //   	{
	   //  			if(destination.length() > 0 && gen->find(destination) == gen->end())
	   //  			{
	   //  				kill->insert(destination);
	   //  			}
	   //  		}
	   //  		else
	   //  		{
	   //  			if(destination.length() > 0 && kill->find(destination) == kill->end())
	   //  			{
	   //  				gen->insert(destination);
	   //  			}
	   //  		}
    // 		}
    // 	}
    // 	block_gen.insert(std::pair<BasicBlock*, std::unordered_set<std::string>*>(bb, gen));
    // 	block_kill.insert(std::pair<BasicBlock*, std::unordered_set<std::string>*>(bb, kill));
    // }

    // void find_fixed_point(Function &F)
    // {
    // 	bool changed = true;
    // 	int num_iter = 0;
    // 	while(changed)
    // 	{
 			// sleep(0.5);
    // 		changed = false;
    // 		num_iter++;
    // 		outs() << "Iteration #" << num_iter << "\n";
    // 		for(BasicBlock &B : F)
    // 		{
	   //  		auto gen = block_gen[&B];
	   //  		auto kill = block_kill[&B];
	   //  		auto in = block_in[&B];
	   //  		auto out = block_out[&B];
	   //  		std::unordered_set<std::string>* new_out = new std::unordered_set<std::string>;
	   //  		std::unordered_set<std::string>* new_in = new std::unordered_set<std::string>;
	   //  		for(BasicBlock* succ : successors(&B)) /*Out = union of all successor ins*/
	   //  		{
	   //  			auto succ_in = block_in[succ];
	   //  			for(auto iterator = succ_in->begin(); iterator != succ_in->end(); iterator++)
			 //      	{
			 //      		new_out->insert(*iterator);
			 //      	}
			 //      	/*Handle phi instructions*/
		  //   		for(Instruction &I : *succ)
		  //   		{
		  //   			if(I.getOpcode() == Instruction::PHI) /*Handle phi instructions separately*/
			 //    		{
			 //    			PHINode* phi = dyn_cast<PHINode>(&I);
			 //    			for (unsigned int index = 0; index < phi->getNumIncomingValues(); index++) 
			 //    			{
	   //                          std::string operand = phi->getIncomingValue(index)->getName();
	   //                          BasicBlock* in_block = phi->getIncomingBlock(index);
	                            
	   //                          if(in_block->getName() == B.getName() && operand.length() > 0)
	   //                          {
	   //                          	new_out->insert(operand);
	   //                          }
	   //                      }
			 //    		}	
		  //   		}

	   //  		}

	   //  		bool out_change = true;
	   //  		if(*new_out == *out)
	   //  		{
	   //  			out_change = false;
	   //  		}
	   //  		if(out_change) /*In will also change*/
	   //  		{
	   //  			changed = true;
	   //  			out->clear();
	   //  			for(auto iterator = new_out->begin(); iterator != new_out->end(); iterator++)
	   //  			{
	   //  				out->insert(*iterator);
	   //  			}
	   //  		}

	   //  		bool in_change = true;
	   //  		for(auto iterator = out->begin(); iterator != out->end(); iterator++)
	   //  		{
	   //  			new_in->insert(*iterator);
	   //  		}
	   //  		for(auto iterator = kill->begin(); iterator != kill->end(); iterator++)
	   //  		{
	   //  			new_in->erase(*iterator);
	   //  		}
	   //  		for(auto iterator = gen->begin(); iterator != gen->end(); iterator++)
	   //  		{
	   //  			new_in->insert(*iterator);
	   //  		}
	   //  		if(*new_in == *in)
	   //  		{
	   //  			in_change = false;
	   //  		}
	   //  		if(in_change)
	   //  		{
	   //  			changed = true;
	   //  			in->clear();
	   //  			for(auto iterator = new_in->begin(); iterator != new_in->end(); iterator++)
	   //  			{
	   //  				in->insert(*iterator);
	   //  			}
	   //  		}
	   //  		delete new_out;
    // 		}	
    // 	}
    // 	outs() << "\n";
    // }

    // void display_live_out(Function& F)
    // {
    // 	outs() << "========== Displaying the live values out from each basic block ==========\n"; 
    // 	for(BasicBlock &B : F)
    //     {
    //     	outs() << "BasicBlock " << B.getName() << ": ";
    //     	auto gen = block_out[&B];
    //     	int num_elems = gen->size();
    //     	int counter = 0;
    //     	for(auto iterator = gen->begin(); iterator != gen->end(); iterator++)
    //     	{
    //     		outs() << *iterator;
    //     		counter++;
    //     		if(counter != num_elems)
    //     		{
    //     			outs() << ", ";
    //     		}
    //     	}
    //     	outs() << "\n";
    //     }
    //     outs() << "==========================================================================\n\n";
    // }

    // void display_live_program_points(Function& F)
    // {
    // 	outs() << "======= Displaying live values at different points in a BasicBlock =======\n";     
    // 	for(BasicBlock &B : F)
    // 	{
    // 		outs() << "BasicBlock " << B.getName() << ":\n";
    // 		int num_points = 0;
    // 		for(Instruction &I : B)
    // 		{
    // 			if(I.getOpcode() == Instruction::PHI)
    // 			{
    // 				continue;
    // 			}
    // 			num_points++;
    // 		}
    // 		num_points++;
    // 		auto out = block_out[&B];
    // 		outs() << "  Program Point " << num_points << ": ";
    // 		unsigned int counter = 0;
    // 		for(auto iterator = out->begin(); iterator != out->end(); iterator++)
    // 		{
    // 			outs() << *iterator;
    // 			counter++;
    // 			if(counter != out->size())
    // 			{
    // 				outs() << ", ";
    // 			}
    // 		}
    // 		outs() << "\n";
    // 		num_points--;
    // 		int num_live = out->size();
    // 		if(num_live_points.find(num_live) == num_live_points.end())
    // 		{
    // 			num_live_points.insert(std::pair<int, int>(num_live, 1));
    // 		}
    // 		else
    // 		{
    // 			num_live_points[num_live] = num_live_points[num_live] + 1;
    // 		}
    // 		if(num_live > max_live)
    // 		{
    // 			max_live = num_live;
    // 		}
    // 		for(auto iter = B.rbegin(); iter != B.rend(); iter++)
    // 		{
    // 			Instruction& I = *iter;
    // 			if(I.getOpcode() == Instruction::PHI)
    // 			{
    // 				continue;
    // 			}
    // 			for (auto operand_it = I.op_begin(); operand_it != I.op_end(); ++operand_it)
		  //   	{
		  //   		std::string operand = (*operand_it)->getName();
		  //   		if(operand.length() > 0 && !(operand.rfind("bb", 0) == 0))
		  //   		{
		  //   			out->insert(operand);
		  //   		}
		  //   	}
		  //   	std::string destination = I.getName();
		  //   	if(destination.length() > 0 && !(destination.rfind("bb", 0) == 0))
		  //   	{
		  //   		out->erase(destination);
		  //   	}
		  //   	outs() << "  Program Point " << num_points << ": ";
		  //   	counter = 0;
	   //  		for(auto iterator = out->begin(); iterator != out->end(); iterator++)
	   //  		{
	   //  			outs() << *iterator;
	   //  			counter++;
	   //  			if(counter != out->size())
	   //  			{
	   //  				outs() << ", ";
	   //  			}
	   //  		}
	   //  		outs() << "\n";
	   //  		num_points--;
	   //  		num_live = out->size();
	   //  		if(num_live_points.find(num_live) == num_live_points.end())
	   //  		{
	   //  			num_live_points.insert(std::pair<int, int>(num_live, 1));
	   //  		}
	   //  		else
	   //  		{
	   //  			num_live_points[num_live] = num_live_points[num_live] + 1;
	   //  		}
	   //  		if(num_live > max_live)
	   //  		{
	   //  			max_live = num_live;
	   //  		}
    // 		}
    // 	}
    // 	outs() << "==========================================================================\n\n";
    // }

    // void display_histogram()
    // {
    // 	outs() << "========== Liveness Histogram -> #live values : #program points ==========\n";
    // 	for(int i = 0; i <= max_live; i++)
    // 	{
    // 		if(num_live_points.find(i) == num_live_points.end())
    // 		{
    // 			outs() << i << " : 0\n";
    // 		}
    // 		else
    // 		{
    // 			outs() << i << " : " << num_live_points[i] << "\n";
    // 		}
    // 	}
    // 	outs() << "==========================================================================\n";
    // }

    bool run_analysis()
    {
      while(!work_list.empty())
      {
        Function* F = *(work_list.begin());
        work_list.erase(work_list.begin());
        for(inst_iterator I = inst_begin(*F); I != inst_end(*F); I++)
        {
          if(I->getOpcode() == Instruction::Load)
          {
            std::string source = I->getOperand(0)->getName();
            std::string destination = I->getName();
            int is_private = 0;
            if(dyn_cast<GlobalVariable>(I->getOperand(0)))
            {
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
              int is_dest_global = 0;
              if(dyn_cast<GlobalVariable>(&*I))
              {
                is_dest_global = 1;
              }
              if(is_dest_pointer == 1 && is_dest_global == 1)
              {
                global_variables.insert(std::pair<std::string, uint8_t>(destination, 1));
                outs() << "Marking global variable " << destination << " a private pointer\n";
              }
              else if(is_dest_pointer == 1 && is_dest_global == 0)
              {
                destination = std::string(F->getName()) + "::" + destination;
                locals.insert(std::pair<std::string, uint8_t>(destination, 1));
                outs() << "Marking local variable " << destination << " a private pointer\n";
              }
              else if(is_dest_pointer == 0 && is_dest_global == 1)
              {
                global_variables.insert(std::pair<std::string, uint8_t>(destination, 0));
                outs() << "Marking global variable " << destination << " private\n";
              }
              else
              {
                destination = std::string(F->getName()) + "::" + destination;
                locals.insert(std::pair<std::string, uint8_t>(destination, 1));
                outs() << "Marking local variable " << destination << " private\n";
              }
            }
          }
          else if(I->getOpcode() == Instruction::Store)
          {
            std::string source = I->getOperand(0)->getName();
            std::string destination = I->getOperand(1)->getName();
            if(source == destination)
            {
              continue;
            }
            int is_private = 0;
            if(dyn_cast<GlobalVariable>(I->getOperand(0)))
            {
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
              if(I->getOperand(1)->getType()->isPointerTy())
              {
                is_dest_pointer = 1;
              }
              int is_dest_global = 0;
              if(dyn_cast<GlobalVariable>(&*(I->getOperand(1))))
              {
                is_dest_global = 1;
              }
              if(is_dest_pointer == 1 && is_dest_global == 1)
              {
                global_variables.insert(std::pair<std::string, uint8_t>(destination, 1));
                outs() << "Marking global variable " << destination << " a private pointer\n";
              }
              else if(is_dest_pointer == 1 && is_dest_global == 0)
              {
                destination = std::string(F->getName()) + "::" + destination;
                locals.insert(std::pair<std::string, uint8_t>(destination, 1));
                outs() << "Marking local variable " << destination << " a private pointer\n";
              }
              else if(is_dest_pointer == 0 && is_dest_global == 1)
              {
                global_variables.insert(std::pair<std::string, uint8_t>(destination, 0));
                outs() << "Marking global variable " << destination << " private\n";
              }
              else
              {
                destination = std::string(F->getName()) + "::" + destination;
                locals.insert(std::pair<std::string, uint8_t>(destination, 1));
                outs() << "Marking local variable " << destination << " private\n";
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
                outs() << "ERROR: Branching on private value " << branch_variable->getName() << "\n";
              }
            }
          }
          else if(I->getOpcode() == Instruction::Ret)
          {

          }
          else if(I->getOpcode() == Instruction::Call)
          {

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
                global_variables.insert(std::pair<std::string, uint8_t>(destination, 1));
                outs() << "Marking global variable " << destination << " a private pointer\n";
              }
              else if(is_dest_pointer == 1 && is_dest_global == 0)
              {
                destination = std::string(F->getName()) + "::" + destination;
                locals.insert(std::pair<std::string, uint8_t>(destination, 1));
                outs() << "Marking local variable " << destination << " a private pointer\n";
              }
              else if(is_dest_pointer == 0 && is_dest_global == 1)
              {
                global_variables.insert(std::pair<std::string, uint8_t>(destination, 0));
                outs() << "Marking global variable " << destination << " private\n";
              }
              else
              {
                destination = std::string(F->getName()) + "::" + destination;
                locals.insert(std::pair<std::string, uint8_t>(destination, 1));
                outs() << "Marking local variable " << destination << " private\n";
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
      // block_in.clear();
      // block_out.clear();
      // block_kill.clear();
      // block_gen.clear();
      // num_live_points.clear();
      // max_live = 0;

      // std::unordered_set<std::string>* variables = all_variables(F);
      // outs() << "List of variables declared: \n";
      // for (auto iterator = variables->begin(); iterator != variables->end(); iterator++)
      // {
      // 	outs() << *iterator << "\n";
      // } 
      // outs() << "\n";

      // for (BasicBlock &B : F) 
      // {
      // 	find_gen_fill(&B);
      // }
      // for(BasicBlock &B : F)
      // {
      // 	outs() << "Gen of BasicBlock " << B.getName() << ": ";
      // 	auto gen = block_gen[&B];
      // 	for(auto iterator = gen->begin(); iterator != gen->end(); iterator++)
      // 	{
      // 		outs() << *iterator << ", ";
      // 	}
      // 	outs() << "\n";
      // 	outs() << "Kill of BasicBlock " << B.getName() << ": ";
      // 	auto kill = block_kill[&B];
      // 	for(auto iterator = kill->begin(); iterator != kill->end(); iterator++)
      // 	{
      // 		outs() << *iterator << ", ";
      // 	}
      // 	outs() << "\n\n";
      // }

      // for(BasicBlock& B : F) /*Initialize ins and outs of every basic block*/
      // {
      // 	std::unordered_set<std::string>* in = new std::unordered_set<std::string>;
      // 	std::unordered_set<std::string>* out = new std::unordered_set<std::string>;
      // 	block_in.insert(std::pair<BasicBlock*, std::unordered_set<std::string>*>(&B, in));
      // 	block_out.insert(std::pair<BasicBlock*, std::unordered_set<std::string>*>(&B, out));
      // }

      // find_fixed_point(F);
      // display_live_out(F);
      // display_live_program_points(F);
      // display_histogram();

      // return false;
    }
  };
}

char SensitivityAnalysis::ID = 0;
static RegisterPass<SensitivityAnalysis> X("sensitive_analysis", "Sensitivity Analysis Pass");