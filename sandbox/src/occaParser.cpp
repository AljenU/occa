#include "occaParser.hpp"

namespace occa {
  namespace parserNamespace {
    parserBase::parserBase(){
      macrosAreInitialized = false;
      globalScope = new statement(*this);
    }

    const std::string parserBase::parseFile(const std::string &filename){
      initMacros();

      const char *cRoot = cReadFile(filename);

      const std::string parsedContent = parseSource(cRoot);

      delete [] cRoot;

      return parsedContent;
    }

    const std::string parserBase::parseSource(const char *cRoot){
      strNode *nodeRoot = splitAndPreprocessContent(cRoot);
      // nodeRoot->print();
      // throw 1;

      loadLanguageTypes();

      globalScope->loadAllFromNode(nodeRoot);
      std::cout << (std::string) *globalScope;
      throw 1;

      markKernelFunctions(*globalScope);
      applyToAllStatements(*globalScope, &parserBase::labelKernelsAsNativeOrNot);

      applyToAllStatements(*globalScope, &parserBase::setupCudaVariables);
      applyToAllStatements(*globalScope, &parserBase::setupOccaVariables);

      applyToAllStatements(*globalScope, &parserBase::setupOccaFors);
      applyToAllStatements(*globalScope, &parserBase::loadVariableInformation);

      addFunctionPrototypes();
      applyToAllStatements(*globalScope, &parserBase::updateConstToConstant);

      addOccaFors();

      // Also auto-adds barriers if needed
      applyToAllStatements(*globalScope, &parserBase::fixOccaForOrder);
      applyToAllStatements(*globalScope, &parserBase::addParallelFors);

      applyToAllStatements(*globalScope, &parserBase::modifyExclusiveVariables);

      modifyOccaForVariables();
      modifyTextureVariables();

      applyToStatementsDefiningVar(&parserBase::addArgQualifiers);

      loadKernelInfos();

      return (std::string) *globalScope;
    }

    //---[ Macro Parser Functions ]-------
    std::string parserBase::getMacroName(const char *&c){
      const char *cStart = c;
      skipWord(cStart);
      skipWhitespace(cStart);
      c = cStart;
      skipWord(c);

      return std::string(cStart, c - cStart);
    }

    bool parserBase::evaluateMacroStatement(const char *&c){
      skipWhitespace(c);

      if(*c == '\0')
        return false;

      strNode *lineNode = new strNode(c);
      applyMacros(lineNode->value);
      strip(lineNode->value);

      strNode *labelNodeRoot = labelCode(lineNode);
      strNode *labelNodePos  = labelNodeRoot;

      // Check if a variable snuck in
      while(labelNodePos){
        if(labelNodePos->type & unknownVariable){
          std::cout << "Variable [" << *labelNodePos << "] not known at compile time.\n";
          throw 1;
        }

        labelNodePos = labelNodePos->right;
      }

      typeHolder th = evaluateLabelNode(labelNodeRoot);

      return (th.doubleValue() != 0);
    }

    typeHolder parserBase::evaluateLabelNode(strNode *labelNodeRoot){
      if(labelNodeRoot->type & presetValue)
        return typeHolder(*labelNodeRoot);

      strNode *labelNodePos = labelNodeRoot;

      while(labelNodePos){
        if(labelNodePos->down.size()){
          // () () is not allowed (down.size is 0 or 1)
          strNode *downNode = labelNodePos->down[0];
          labelNodePos->down.clear();

          if(labelNodePos->type)
            labelNodePos = labelNodePos->push( evaluateLabelNode(downNode) );
          else
            labelNodePos->value = evaluateLabelNode(downNode);

          labelNodePos->type = presetValue;
        }

        if(labelNodePos->right == NULL)
          break;

        labelNodePos = labelNodePos->right;
      }

      strNode *labelNodeEnd = labelNodePos;

      if(labelNodeEnd && labelNodeRoot){
        if((labelNodeRoot->type & startParentheses) &&
           (labelNodeEnd->type  & endParentheses)){

          popAndGoRight(labelNodeRoot);
          labelNodeEnd->pop();
        }
      }

      strNode *minOpNode;
      int minPrecedence, minOpType;

      labelNodePos = labelNodeRoot;

      while(true){
        minOpNode     = NULL;
        minPrecedence = 100;
        minOpType     = -1;

        while(labelNodePos){
          if(labelNodePos->type & operatorType){
            int opType = (labelNodePos->type & operatorType);

            opType &= ~qualifierType;

            if(opType & unitaryOperatorType){
              if((opType & binaryOperatorType) && // + and - operators
                 (labelNodePos->left)          &&
                 (labelNodePos->left->type & presetValue)){

                opType = binaryOperatorType;
              }
              else if((opType & rUnitaryOperatorType) &&
                      (labelNodePos->left)            &&
                      (labelNodePos->left->type & presetValue)){

                opType = rUnitaryOperatorType;
              }
              else if((opType & lUnitaryOperatorType) &&
                      (labelNodePos->right)           &&
                      (labelNodePos->right->type & presetValue)){

                opType = lUnitaryOperatorType;
              }
              else
                opType &= ~unitaryOperatorType;
            }

            const int opP = opPrecedence[opHolder(labelNodePos->value,
                                                  opType)];

            if(opP < minPrecedence){
              minOpType     = opType;
              minOpNode     = labelNodePos;
              minPrecedence = opP;
            }
          }

          labelNodePos = labelNodePos->right;
        }

        if(minOpNode == NULL){
          if(labelNodeRoot && (labelNodeRoot->right == NULL))
            return typeHolder(*labelNodeRoot);

          std::cout << "5. Error on:\n";
          labelNodeRoot->print("  ");
          throw 1;
        }
        else{
          if(minOpType & unitaryOperatorType){
            if(minOpType & lUnitaryOperatorType){
              std::string op = minOpNode->value;
              std::string a  = minOpNode->right->value;

              minOpNode->value = applyOperator(op, a);
              minOpNode->type  = presetValue;

              minOpNode->right->pop();
            }
            else if(minOpType & rUnitaryOperatorType){
              std::cout << "Postfix operator [" << *minOpNode << "] cannot be used in a macro.\n";
              throw 1;
            }
          }
          else if(minOpType & binaryOperatorType){
            minOpNode = minOpNode->left;

            std::string a  = minOpNode->value;
            std::string op = minOpNode->right->value;
            std::string b  = minOpNode->right->right->value;

            minOpNode->value = applyOperator(a, op, b);
            minOpNode->type  = presetValue;

            minOpNode->right->pop();
            minOpNode->right->pop();
          }
          else if(minOpType & ternaryOperatorType){
            minOpNode = minOpNode->left;

            std::string a  = minOpNode->value;
            std::string op = minOpNode->right->value;
            std::string b  = minOpNode->right->right->value;
            std::string c  = minOpNode->right->right->right->right->value;

            minOpNode->value = applyOperator(a, op, b, c);
            minOpNode->type  = presetValue;

            minOpNode->right->pop();
            minOpNode->right->pop();
            minOpNode->right->pop();
            minOpNode->right->pop();
          }
        }

        if(labelNodeRoot->right == NULL)
          return typeHolder(*labelNodeRoot);

        labelNodePos = labelNodeRoot;
      }

      // Shouldn't get here
      typeHolder th(labelNodeRoot->value);

      return th;
    }

    void parserBase::loadMacroInfo(macroInfo &info, const char *&c){
      skipWhitespace(c);

      info.argc = 0;
      info.parts.clear();
      info.argBetweenParts.clear();

      info.parts.push_back(""); // First part

      info.isAFunction = false;

      if(*c == '\0')
        return;

      if(*c != '('){
        const size_t chars = strlen(c);

        info.parts[0] = strip(c, chars);

        c += chars;

        return;
      }

      int partPos = 0;
      info.isAFunction = true;

      ++c; // '('

      typedef std::map<std::string,int> macroArgMap_t;
      typedef macroArgMap_t::iterator macroArgMapIterator;
      macroArgMap_t macroArgMap;

      while(*c != '\0'){
        skipWhitespace(c);
        const char *cStart = c;
        skipWord(c);

        macroArgMap[std::string(cStart, c - cStart)] = (info.argc++);

        skipWhitespace(c);

        if(*(c++) == ')')
          break;
      }

      skipWhitespace(c);

      while(*c != '\0'){
        const char *cStart = c;

        if(isAString(c)){
          skipString(c);

          info.parts[partPos] += std::string(cStart, (c - cStart));
          continue;
        }

        const int delimeterChars = skipWord(c);

        std::string word = std::string(cStart, c - cStart);

        macroArgMapIterator it = macroArgMap.find(word);

        if(it == macroArgMap.end())
          info.parts[partPos] += word;
        else{
          info.argBetweenParts.push_back(it->second);
          info.parts.push_back("");
          ++partPos;
        }

        cStart = c;
        c += delimeterChars;

        if(cStart != c)
          info.parts[partPos] += std::string(cStart, c - cStart);

        skipWhitespace(c);
      }
    }

    int parserBase::loadMacro(const std::string &line, const int state){
      const char *c = (line.c_str() + 1); // 1 = #

      while(*c != '\0'){
        skipWhitespace(c);
        const char *cEnd = c;
        skipToWhitespace(cEnd);

        if(stringsAreEqual(c, (cEnd - c), "if")){
          c = cEnd;

          bool isTrue = evaluateMacroStatement(c);

          if(isTrue)
            return (startHash | readUntilNextHash);
          else
            return (startHash | ignoreUntilNextHash);
        }
        else if(stringsAreEqual(c, (cEnd - c), "elif")){
          if((state & readUntilNextHash) || (state & ignoreUntilEnd))
            return ignoreUntilEnd;

          c = cEnd;

          bool isTrue = evaluateMacroStatement(c);

          if(isTrue)
            return readUntilNextHash;
          else
            return ignoreUntilNextHash;
        }
        else if(stringsAreEqual(c, (cEnd - c), "else")){
          if((state & readUntilNextHash) || (state & ignoreUntilEnd))
            return ignoreUntilEnd;
          else
            return readUntilNextHash;
        }
        else if(stringsAreEqual(c, (cEnd - c), "ifdef")){
          std::string name = getMacroName(c);

          if(macroMap.find(name) != macroMap.end())
            return (startHash | ignoreUntilNextHash);
          else
            return (startHash | readUntilNextHash);
        }
        else if(stringsAreEqual(c, (cEnd - c), "ifndef")){
          std::string name = getMacroName(c);

          if(macroMap.find(name) != macroMap.end())
            return (startHash | readUntilNextHash);
          else
            return (startHash | ignoreUntilNextHash);
        }
        else if(stringsAreEqual(c, (cEnd - c), "endif")){
          return doneIgnoring;
        }
        else if(stringsAreEqual(c, (cEnd - c), "define")){
          if(state & ignoring)
            return state;

          std::string name = getMacroName(c);
          int pos;

          if(macroMap.find(name) == macroMap.end()){
            pos = macros.size();
            macros.push_back( macroInfo() );
            macroMap[name] = pos;
          }
          else
            pos = macroMap[name];

          macroInfo &info = macros[pos];
          info.name = name;

          loadMacroInfo(info, c);

          return state;
        }
        else if(stringsAreEqual(c, (cEnd - c), "undef")){
          if(state & ignoring)
            return state;

          std::string name = getMacroName(c);

          if(macroMap.find(name) != macroMap.end())
            macroMap.erase(name);

          return state;
        }
        else if(stringsAreEqual(c, (cEnd - c), "include")){
          if(state & ignoring)
            return state;
        }
        else if(stringsAreEqual(c, (cEnd - c), "pragma"))
          return (state | keepMacro);
        else
          return (state | keepMacro);

        c = cEnd;
      }

      return state;
    }

    void parserBase::applyMacros(std::string &line){
      const char *c = line.c_str();
      std::string newLine = "";

      bool foundMacro = false;

      while(*c != '\0'){
        const char *cStart = c;

        if(isAString(c)){
          skipString(c);

          newLine += std::string(cStart, (c - cStart));
          continue;
        }

        int delimeterChars = skipWord(c);

        std::string word = std::string(cStart, c - cStart);

        macroMapIterator it = macroMap.find(word);

        if((delimeterChars == 2) &&
           stringsAreEqual(c, delimeterChars, "##") &&
           it != macroMap.end()){
          macroInfo &info = macros[it->second];
          word = info.parts[0];
        }

        while((delimeterChars == 2) &&
              stringsAreEqual(c, delimeterChars, "##")){
          c += 2;

          cStart = c;
          delimeterChars = skipWord(c);

          std::string word2 = std::string(cStart, c - cStart);

          it = macroMap.find(word2);

          if(it != macroMap.end()){
            macroInfo &info = macros[it->second];
            word += info.parts[0];
          }
          else
            word += word2;
        }

        it = macroMap.find(word);

        if(it != macroMap.end()){
          foundMacro = true;

          macroInfo &info = macros[it->second];

          if(!info.isAFunction)
            newLine += info.parts[0];

          else{
            std::vector<std::string> args;

            ++c; // '('

            while(*c != '\0'){
              skipWhitespace(c);
              cStart = c;
              skipWord(c);

              args.push_back( std::string(cStart, c - cStart) );

              skipWhitespace(c);

              if(*(c++) == ')')
                break;
            }

            newLine += info.applyArgs(args);
          }
        }
        else
          newLine += word;

        cStart = c;
        c += delimeterChars;

        if(cStart != c)
          newLine += std::string(cStart, c - cStart);

        if(isWhitespace(*c)){
          newLine += ' ';
          skipWhitespace(c);
        }
      }

      line = newLine;

      if(foundMacro)
        applyMacros(line);
    }

    strNode* parserBase::preprocessMacros(strNode *nodeRoot){
      strNode *nodePos  = nodeRoot;

      std::stack<int> statusStack;

      int currentState = doNothing;

      while(nodePos){
        std::string &line = nodePos->value;
        bool ignoreLine = false;

        if(line[0] == '#'){
          const int oldState = currentState;

          currentState = loadMacro(line, currentState);

          if(currentState & keepMacro){
            currentState &= ~keepMacro;

            if( !(currentState & ignoring) )
              nodePos->type = macroKeywordType;
          }
          else{
            // Nested #if's
            if(currentState & startHash){
              currentState &= ~startHash;
              statusStack.push(oldState);
            }

            if(currentState & doneIgnoring){
              if(statusStack.size()){
                currentState = statusStack.top();
                statusStack.pop();
              }
              else
                currentState = doNothing;
            }

            ignoreLine = true;
          }
        }
        else{
          if(!(currentState & ignoring))
            applyMacros(line);
          else
            ignoreLine = true;
        }

        if(ignoreLine){
          if(nodeRoot == nodePos)
            nodeRoot = nodePos->right;

          popAndGoRight(nodePos);
        }
        else
          nodePos = nodePos->right;
      }

      return nodeRoot;
    }

    strNode* parserBase::splitAndPreprocessContent(const char *cRoot){
      initKeywords();
      initMacros();

      strNode *nodeRoot = splitContent(cRoot);

      nodeRoot = preprocessMacros(nodeRoot);
      nodeRoot = labelCode(nodeRoot);

      return nodeRoot;
    }
    //====================================

    void parserBase::initMacros(){
      if(!macrosAreInitialized)
        return;

      macrosAreInitialized = true;

      //---[ Macros ]---------------------
      loadMacro("#define kernel occaKernel");

      loadMacro("#define barrier        occaBarrier");
      loadMacro("#define localMemFence  occaLocalMemFence");
      loadMacro("#define globalMemFence occaGlobalMemFence");

      loadMacro("#define shared   occaShared");
      loadMacro("#define restrict occaRestrict");
      loadMacro("#define volatile occaVolatile");
      loadMacro("#define aligned  occaAligned");
      loadMacro("#define const    occaConst");
      loadMacro("#define constant occaConstant");

      std::string mathFunctions[16] = {
        "sqrt", "sin"  , "asin" ,
        "sinh", "asinh", "cos"  ,
        "acos", "cosh" , "acosh",
        "tan" , "atan" , "tanh" ,
        "atanh", "exp" , "log2" ,
        "log10"
      };

      for(int i = 0; i < 16; ++i){
        std::string mf = mathFunctions[i];
        std::string cmf = mf;
        cmf[0] += ('A' - 'a');

        loadMacro("#define "       + mf  + " occa"       + cmf);
        loadMacro("#define fast"   + cmf + " occaFast"   + cmf);
        loadMacro("#define native" + cmf + " occaNative" + cmf);
      }

      //---[ CUDA Macros ]----------------
      loadMacro("#define __global__ occaKernel");

      loadMacro("#define __syncthreads()       occaBarrier(occaGlobalMemFence)");
      loadMacro("#define __threadfence_block() occaBarrier(occaLocalMemFence)");
      loadMacro("#define __threadfence()       occaBarrier(occaGlobalMemFence)");

      loadMacro("#define __shared__   occaShared");
      loadMacro("#define __restrict__ occaRestrict");
      loadMacro("#define __volatile__ occaVolatile");
      loadMacro("#define __constant__ occaConstant");

      loadMacro("#define __device__ occaFunction");

      //---[ OpenCL Macros ]--------------
      loadMacro("#define __kernel occaKernel");

      loadMacro("#define CLK_LOCAL_MEM_FENCE  occaLocalMemFence");
      loadMacro("#define CLK_GLOBAL_MEM_FENCE occaGlobalMemFence");

      loadMacro("#define __local    occaShared");
      loadMacro("#define __global   occaPointer");
      loadMacro("#define __constant occaConstant");

      loadMacro("#define get_num_groups(X)  occaOuterDim##X");
      loadMacro("#define get_group_id(X)    occaOuterId##X ");
      loadMacro("#define get_local_size(X)  occaInnerDim##X");
      loadMacro("#define get_local_id(X)    occaInnerId##X ");
      loadMacro("#define get_global_size(X) occaGlobalDim##X");
      loadMacro("#define get_global_id(X)   occaGlobalId##X ");
    }

    void parserBase::loadLanguageTypes(){
      int parts[6]            = {1, 2, 3, 4, 8, 16};
      std::string suffix[6]   = {"", "2", "3", "4", "8", "16"};
      std::string baseType[7] = {"int"  ,
                                 "bool" ,
                                 "char" ,
                                 "long" ,
                                 "short",
                                 "float",
                                 "double"};

      for(int t = 0; t < 7; ++t){
        for(int n = 0; n < 6; ++n){
          typeDef &def = *(new typeDef);
          def.typeName = baseType[t] + suffix[n];

          globalScope->scopeTypeMap[def.typeName] = &def;

          if(n){
            def.typeInfo = structTypeDef;

            for(int n2 = 0; n2 < parts[n]; ++n2){
              typeDef &uDef = *(new typeDef);
              uDef.typeInfo = unionTypeDef;

              if(n2 < 4){
                std::string varName = "w";
                varName[0] += ((n2 + 1) % 4);

                typeDef &sDef = uDef.addType(varName);
                sDef.typeName = baseType[t];
              }

              if(n2 < 10){
                std::string varName = "s";
                varName += '0' + n2;

                typeDef &sDef = (n2 < 4) ? uDef.addType(varName) : def.addType(varName);
                sDef.typeName = baseType[t];
              }
              else{
                std::string varName = "s";

                typeDef &sDef1 = uDef.addType(varName + (char) ('a' + (n2 - 10)));
                sDef1.typeName = baseType[t];

                typeDef &sDef2 = uDef.addType(varName + (char) ('A' + (n2 - 10)));
                sDef2.typeName = baseType[t];
              }

              if((n2 < 4) || (10 <= n2))
                def.addType(&uDef);
              else
                delete &uDef;
            }
          }
        }
      }

      globalScope->addTypeDef("void");
      globalScope->addTypeDef("__builtin_va_list");
    }

    void parserBase::applyToAllStatements(statement &s,
                                          applyToAllStatements_t func){
      (this->*func)(s);

      statementNode *statementPos = s.statementStart;

      while(statementPos){
        applyToAllStatements(*(statementPos->value), func);
        statementPos = statementPos->right;
      }
    }

    void parserBase::applyToStatementsDefiningVar(applyToStatementsDefiningVar_t func){
      varOriginMapIterator it = varOriginMap.begin();

      while(it != varOriginMap.end()){
        (this->*func)(*(it->first), *(it->second));

        ++it;
      }
    }

    void parserBase::applyToStatementsUsingVar(varInfo &info,
                                               applyToStatementsUsingVar_t func){
      varUsedMapIterator it = varUsedMap.find(&info);

      if(it != varUsedMap.end()){
        statementNode *sn = it->second.right;

        while(sn){
          (this->*func)(info, *(sn->value));

          sn = sn->right;
        }
      }
    }

    bool parserBase::statementIsAKernel(statement &s){
      if(s.type & functionStatementType){
        strNode *nodePos = s.nodeStart;

        while(nodePos){
          if(nodePos->value == "occaKernel")
            return true;

          nodePos = nodePos->right;
        }
      }

      return false;
    }

    statement* parserBase::getStatementKernel(statement &s){
      statement *sUp = &s;

      while(sUp){
        if(statementIsAKernel(*sUp))
          return sUp;

        sUp = sUp->up;
      }

      return sUp;
    }

    bool parserBase::statementKernelUsesNativeOCCA(statement &s){
      statement *sKernel = getStatementKernel(s);

      if(sKernel == NULL)
        return false;

      std::string check = obfuscate("native", "occa");
      varInfo *info = sKernel->hasVariableInScope(check);

      if(info != NULL)
        return true;
      else
        return false;
    }

    bool parserBase::statementKernelUsesNativeOKL(statement &s){
      statement *sKernel = getStatementKernel(s);

      if(sKernel == NULL)
        return false;

      std::string check = obfuscate("native", "okl");
      varInfo *info = sKernel->hasVariableInScope(check);

      if(info != NULL)
        return true;
      else
        return false;
    }

    bool parserBase::statementKernelUsesNativeLanguage(statement &s){
      if(statementKernelUsesNativeOKL(s))
        return true;

      if(statementKernelUsesNativeOCCA(s))
        return true;

      return false;
    }

    void parserBase::addOccaForCounter(statement &s,
                                       const std::string &ioLoop,
                                       const std::string &loopNest){
      varInfo ioDimVar;
      ioDimVar.name = obfuscate(ioLoop);
      ioDimVar.extraInfo.push_back(loopNest);

      varInfo *ioDimVar2 = s.hasVariableInScope(ioDimVar.name);

      if(ioDimVar2 == NULL){
        statement &sKernel = *(getStatementKernel(s));
        sKernel.addVariable(ioDimVar);
      }
      else{
        const int extras = ioDimVar2->extraInfo.size();
        int i;

        for(i = 0; i < extras; ++i){
          if(ioDimVar2->extraInfo[i] == loopNest)
            break;
        }

        if(i == extras)
          ioDimVar2->extraInfo.push_back(loopNest);
      }
    }

    bool parserBase::nodeHasUnknownVariable(strNode *n){
      while(n){
        if(n->type & unknownVariable)
          return true;

        const int downCount = n->down.size();

        for(int i = 0; i < downCount; ++i)
          if(nodeHasUnknownVariable(n->down[i]))
            return true;

        n = n->right;
      }

      return false;
    }

    void parserBase::setupOccaFors(statement &s){
      if( !(s.type & forStatementType) )
        return;

      statement *spKernel = getStatementKernel(s);

      if(spKernel == NULL)
        return;

      if(statementKernelUsesNativeOCCA(s))
        return;

      statement &sKernel = *spKernel;

      strNode *nodePos = s.nodeStart;
      strNode *lastNode;

      while(nodePos->down.size() == 0)
        nodePos = nodePos->right;

      lastNode = nodePos;
      nodePos  = nodePos->down[0]->right;

      std::vector<strNode*> oldDown(lastNode->down.begin() + 1,
                                    lastNode->down.end());

      // Last segment doesn't have a [;]
      int segmentCount = 1;
      strNode *commaNodes[4];

      // First statement
      commaNodes[0] = nodePos;

      while(nodePos){
        if(nodePos->value == ";"){
          if(segmentCount == 4){
            std::cout << "More than 4 statements for:\n  " << s.expRoot << '\n';
            throw 1;
          }

          commaNodes[segmentCount++] = nodePos;
        }

        nodePos = nodePos->right;
      }

      if(segmentCount < 4)
        return;

      nodePos = commaNodes[3]->right;

      // If it has a fourth argument, make sure it's the correct one
      if( ((nodePos->value.find("inner") == std::string::npos) ||
           ((nodePos->value != "inner0") &&
            (nodePos->value != "inner1") &&
            (nodePos->value != "inner2")))                     &&
          ((nodePos->value.find("outer") == std::string::npos) ||
           ((nodePos->value != "outer0") &&
            (nodePos->value != "outer1") &&
            (nodePos->value != "outer2"))) ){

        std::cout << "Wrong 4th statement for:\n  " << s.expRoot << '\n';
        throw 1;
      }

      // [-----][#]
      std::string ioLoop   = nodePos->value.substr(0,5);
      std::string loopNest = nodePos->value.substr(5,1);

      ioLoop[0] += ('A' - 'a');

      addOccaForCounter(s, ioLoop, loopNest);

      varInfo idVar, dimVar, loopVar;
      std::string idName, dimName, dimSubName, loopName;

      nodePos = commaNodes[0];
      idVar = s.loadVarInfo(nodePos);

      idName = idVar.name;
      idVar.extraInfo.push_back("occa" + ioLoop + "Id" + loopNest);

      if(nodePos->value != "="){
        std::cout << "The first statement of:\n"
                  << "  " << s.expRoot << '\n'
                  << "should look like:\n"
                  << "  for(int var = off; ...; ...; ...)\n";
        throw 1;
      }

      nodePos = nodePos->right;

      strNode *idOffStart = nodePos->clone();
      strNode *idOffEnd   = idOffStart;

      while(nodePos != commaNodes[1]){
        if( nodePos->hasType(unknownVariable) ){
          std::cout << "The first statement of:\n"
                    << "  " << s.expRoot << '\n'
                    << "should look like:\n"
                    << "  for(int var = off; ...; ...; ...)\n"
                    << "where [off] needs to be known at compile time\n";
          throw 1;
        }

        nodePos  = nodePos->right;
        idOffEnd = idOffEnd->right;
      }

      // Go behind [;]
      idOffEnd = idOffEnd->left;
      // Don't kill me
      idOffEnd->left = NULL;
      free(idOffEnd);

      idVar.extraInfo.push_back( s.prettyString(idOffStart) );

      // [-] ?
      if(!sKernel.hasVariableInScope(idVar.name))
        sKernel.addVariable(idVar);

      s.addVariable(idVar);

      nodePos = nodePos->right;

      if(nodePos->value != idName){
        std::cout << "The second statement of:\n"
                  << "  " << s.expRoot << '\n'
                  << "should look like:\n"
                  << "  for(int [var] = off; [var] < dim; ...; ...)\n"
                  << "           ^____________^ are the same\n";
        throw 1;
      }

      nodePos = nodePos->right;

      if(nodePos->value != "<"){
        std::cout << "The second statement of:\n"
                  << "  " << s.expRoot << '\n'
                  << "should look like:\n"
                  << "  for(int var = off; var [<] dim; ...; ...)\n"
                  << "                          ^ less than\n";
        throw 1;
      }

      nodePos = nodePos->right;

      // Dim is shadowed by a variable
      if((nodePos->type & unknownVariable) &&
         (nodePos->right == commaNodes[2])){

        dimName = nodePos->value;
      }
      // Dim explicitly stated
      else{
        bool startFromBelow = false;

        // blah < [(...) ...]
        if(nodePos->left->down.size()){
          nodePos = nodePos->left;
          startFromBelow = true;
        }

        // Un-link node chain
        commaNodes[2]->left->right = NULL;

        // Manual bounds
        bool errorFound = nodeHasUnknownVariable(nodePos);

        if(!errorFound){
          strNode *tmpNodePos;

          if(startFromBelow){
            tmpNodePos = nodePos->clone();

            tmpNodePos->value = "+";
            tmpNodePos->type  = keywordType["+"];
          }
          else
            tmpNodePos = nodePos;

          dimSubName = (std::string) evaluateLabelNode(tmpNodePos);

          // Re-link node chain
          commaNodes[2]->left->right = commaNodes[2]->left;
        }
        else{
          std::cout << "The second statement of:\n"
                    << "  " << s.expRoot << '\n'
                    << "should look like:\n"
                    << "  for(int var = off; var < [dim]; ...; ...)\n"
                    << "                            ^ only one variable name\n"
                    << "                              or exact value\n";
          throw 1;
        }
      }

      if(dimName.size())
        dimVar.name = dimName;
      else
        dimVar.name = obfuscate("occa" + ioLoop + "Dim" + loopNest);

      if(dimSubName.size())
        dimVar.extraInfo.push_back(dimSubName);
      else
        dimVar.extraInfo.push_back("occa" + ioLoop + "Dim" + loopNest);

      // [-] ?
      if(!sKernel.hasVariableInScope(dimVar.name))
        sKernel.addVariable(dimVar);

      s.addVariable(dimVar);

      nodePos = commaNodes[3]->right;

      loopVar.name = "occa" + ioLoop + "For" + loopNest;

      s.addVariable(loopVar);

      std::string occaForName = "occa" + ioLoop + "For" + loopNest;

      // [-] Needs proper free (can't because it's nested...)
      // free(s.nodeStart);

      s.type            = keywordType["occaOuterFor0"];
      s.nodeStart       = new strNode(occaForName);
      s.nodeStart->type = s.type;

      s.nodeEnd = s.nodeStart;

      const int downCount = oldDown.size();

      for(int i = 0; i < downCount; ++i){
        // Remove the braces
        popAndGoRight(oldDown[i]);
        parserNamespace::lastNode(oldDown[i])->pop();

        s.loadAllFromNode(oldDown[i]);
      }
    }

    void parserBase::loadScopeVarMap(statement &s){
      if((!(s.type & declareStatementType)   &&
          !(s.type & structStatementType)    &&
          !(s.type & forStatementType)       &&
          !(s.type & gotoStatementType)      &&
          !(s.type & functionStatementType)) ||
         // OCCA for's don't have arguments
         (s.type == (forStatementType | occaStatementType)))
        return;

      // Check for struct defs
#if 1
      if(s.type & structStatementType)
        return;
#else
      if(s.type & structStatementType){
        if(s.typePtr->varName.size()){
          typeDef &type = *(s.typePtr);
          varInfo info;

          info.type = &type;
          info.name = type.varName;

          info.pointerCount      = type.pointerCount;
          info.stackPointerSizes = type.stackPointerSizes;

          (s.up)->addVariable(info, &s);
        }

        return;
      }
#endif

      strNode *nodePos = s.nodeStart;

      statement *up = s.up;

      if(s.type & functionStatementType){
        varInfo info = s.loadVarInfo(nodePos);
        (s.up)->addVariable(info, &s);
      }

      if(s.type & (forStatementType |
                   functionStatementType)){
        while(nodePos->down.size() == 0)
          nodePos = nodePos->right;

        nodePos = nodePos->down[0]->right;

        up = &s;
      }

      if( !(s.type & functionPrototypeType) ){
        varInfo info = s.loadVarInfo(nodePos);

        if((info.type == NULL) ||
           (info.typeInfo & functionCallType))
          return;

        up->addVariable(info, &s);

        while(nodePos){
          if(nodePos->value == ","){
            nodePos = nodePos->right;

            varInfo info2 = s.loadVarInfo(nodePos);

            // Functions have types for each argument
            if( !(s.type & functionStatementType) ){
              info2.type        = info.type;
              info2.descriptors = info.descriptors;
            }

            up->addVariable(info2, &s);
          }
          else
            nodePos = nodePos->right;
        }
      }
    }

    bool parserBase::statementHasOccaOuterFor(statement &s){
      if(s.type == keywordType["occaOuterFor0"]){
        std::string &forName = s.nodeStart->value;

        if((forName.find("occaOuterFor") != std::string::npos) &&
           ((forName == "occaOuterFor0") ||
            (forName == "occaOuterFor1") ||
            (forName == "occaOuterFor2"))){

          return true;
        }
      }

      statementNode *statementPos = s.statementStart;

      while(statementPos){
        if( statementHasOccaOuterFor(*(statementPos->value)) )
          return true;

        statementPos = statementPos->right;
      }

      return false;
    }

    bool parserBase::statementHasOccaFor(statement &s){
      if(s.type == keywordType["occaOuterFor0"])
        return true;

      statementNode *statementPos = s.statementStart;

      while(statementPos){
        if( statementHasOccaFor(*(statementPos->value)) )
          return true;

        statementPos = statementPos->right;
      }

      return false;
    }

    bool parserBase::statementHasOklFor(statement &s){
      if(s.type == forStatementType){

        strNode *nodePos = s.nodeStart;

        while(nodePos->down.size() == 0)
          nodePos = nodePos->right;

        nodePos = nodePos->down[0];

        while(nodePos){
          if( ((nodePos->value.find("inner") != std::string::npos) &&
               ((nodePos->value == "inner0") ||
                (nodePos->value == "inner1") ||
                (nodePos->value == "inner2")))                     ||
              ((nodePos->value.find("outer") != std::string::npos) &&
               ((nodePos->value == "outer0") ||
                (nodePos->value == "outer1") ||
                (nodePos->value == "outer2"))) ){

            return true;
          }

          nodePos = nodePos->right;
        }
      }

      statementNode *statementPos = s.statementStart;

      while(statementPos){
        if( statementHasOklFor(*(statementPos->value)) )
          return true;

        statementPos = statementPos->right;
      }

      return false;
    }

    bool parserBase::statementHasOccaStuff(statement &s){
      if(statementHasOklFor(s))
        return true;

      if(statementHasOccaOuterFor(s))
        return true;

      statementNode *statementPos = s.statementStart;

      while(statementPos){
        if( statementHasOccaStuff(*(statementPos->value)) )
          return true;

        statementPos = statementPos->right;
      }

      return false;
    }

    void parserBase::markKernelFunctions(statement &s){
      statementNode *snPos = s.statementStart;

      while(snPos){
        statement &s2 = *(snPos->value);

        if( !(s2.type & functionStatementType) ||
            statementIsAKernel(s2) ){

          snPos = snPos->right;
          continue;
        }

        if(statementHasOccaStuff(s2)){
          strNode *kernelMark = new strNode("occaKernel");
          kernelMark->type    = keywordType["occaKernel"];

          s2.nodeStart->left = kernelMark;
          kernelMark->right  = s2.nodeStart;

          s2.nodeStart = kernelMark;
        }

        snPos = snPos->right;
      }
    }

    void parserBase::labelKernelsAsNativeOrNot(statement &s){
      if(!statementIsAKernel(s))
        return;

      bool hasOccaFor = statementHasOccaFor(s);
      bool hasOklFor  = statementHasOklFor(s);

      if(hasOccaFor | hasOklFor){
        varInfo nativeCheckVar;

        if(hasOccaFor)
          nativeCheckVar.name = obfuscate("native", "occa");
        else
          nativeCheckVar.name = obfuscate("native", "okl");

        varInfo *nativeCheckVar2 = s.hasVariableInScope(nativeCheckVar.name);

        if(nativeCheckVar2 == NULL)
          s.addVariable(nativeCheckVar);
      }
    }

    void parserBase::setupCudaVariables(statement &s){
      if((!(s.type & declareStatementType)   &&
          !(s.type & forStatementType)       &&
          !(s.type & functionStatementType)) ||
         // OCCA for's don't have arguments
         (s.type == keywordType["occaOuterFor0"]))
        return;

      if(getStatementKernel(s) == NULL)
        return;

      if(statementKernelUsesNativeLanguage(s))
        return;

      strNode *nodePos = s.nodeStart;

      statement *up = s.up;

      // Go to [(]
      if(s.type & functionStatementType)
        s.loadVarInfo(nodePos);

      if(s.type & (forStatementType |
                   functionStatementType)){
        while(nodePos->down.size() == 0)
          nodePos = nodePos->right;

        nodePos = nodePos->down[0]->right;

        up = &s;
      }

      if( !(s.type & functionPrototypeType) ){
        while(nodePos){
          if((nodePos->type & cudaKeywordType) &&
             (nodePos->right)                  &&
             (nodePos->right->value == ".")    &&
             (nodePos->right->right)){

            std::string &coord = nodePos->right->right->value;

            if((coord.size() == 1) &&
               ('x' <= coord[0]) && (coord[0] <= 'z')){
              std::string occaCoord = coord.substr(0,1);
              occaCoord[0] += ('0' - 'x');

              bool compressing = false;

              if(nodePos->value == "threadIdx"){
                compressing = true;
                nodePos->value = "occaInnerId" + occaCoord;
              }
              else if(nodePos->value == "blockDim"){
                compressing = true;
                nodePos->value = "occaInnerDim" + occaCoord;
              }
              else if(nodePos->value == "blockIdx"){
                compressing = true;
                nodePos->value = "occaOuterId" + occaCoord;
              }
              else if(nodePos->value == "gridDim"){
                compressing = true;
                nodePos->value = "occaOuterDim" + occaCoord;
              }

              if(compressing){
                nodePos->type = keywordType[nodePos->value];
                nodePos->right->pop();
                nodePos->right->pop();
              }
            }
          }

          if(nodePos != NULL)
            nodePos = nodePos->right;
        }
      }
    }

    void parserBase::loadVariableInformation(statement &s,
                                             strNode *n){
      if(s.type & (functionPrototypeType |
                   structStatementType))
        return;

      // Skip qualifiers (good for function pointers)
      if(s.type & declareStatementType)
        s.loadVarInfo(n);

      while(n){
        if((n->type & unknownVariable) &&
           !s.hasTypeInScope(n->value)){

          varInfo *infoPtr = s.hasVariableInScope(n->value);

          if(infoPtr == NULL){
            std::cout << "Couldn't find [" << (std::string) *n << "] in:\n"
                      << s << '\n';
            throw 1;
          }

          varUsedMap[infoPtr].push(&s);
        }

        const int downCount = n->down.size();

        for(int i = 0; i < downCount; ++i)
          loadVariableInformation(s, n->down[i]);

        n = n->right;
      }
    }

    void parserBase::loadVariableInformation(statement &s){
      if(s.type & macroStatementType)
        return;

      loadScopeVarMap(s);

      loadVariableInformation(s, s.nodeStart);
    }

    void parserBase::addFunctionPrototypes(){
      std::map<std::string,bool> prototypes;

      statementNode *statementPos = globalScope->statementStart;

      while(statementPos){
        statement *s2 = statementPos->value;

        if(s2->type & functionPrototypeType){
          strNode *nodePos = s2->nodeStart;
          varInfo info = globalScope->loadVarInfo(nodePos);

          prototypes[info.name] = true;
        }

        statementPos = statementPos->right;
      }

      statementPos = globalScope->statementStart;

      while(statementPos){
        statement *s2 = statementPos->value;

        if(s2->type & functionStatementType){
          strNode *nodePos = s2->nodeStart;
          varInfo info = globalScope->loadVarInfo(nodePos);

          if(info.hasDescriptor("occaKernel") ||
             info.hasDescriptor("kernel")){

            statementPos = statementPos->right;
            continue;
          }

          if(!info.hasDescriptor("occaFunction")){
            strNode *ofNode = s2->nodeStart;

            ofNode       = ofNode->push("occaFunction");
            ofNode->type = keywordType["occaFunction"];

            ofNode->swapWithLeft();
            s2->nodeStart = ofNode;
          }

          if( !(s2->type & functionDefinitionType) ){
            statementPos = statementPos->right;
            continue;
          }

          if(prototypes.find(info.name) == prototypes.end()){
            statement *newS2 = s2->clone();
            statementNode *newNode = new statementNode(newS2);

            newS2->type = functionPrototypeType;

            newS2->statementCount = 0;

            // [-] Delete definition (needs proper free)
            delete newS2->statementStart;
            newS2->statementStart = NULL;
            newS2->statementEnd   = NULL;

            strNode *end = newS2->nodeEnd;
            end = end->push(";");

            end->up        = newS2->nodeEnd->up;
            end->type      = keywordType[";"];
            end->depth     = newS2->nodeEnd->depth;
            end->sideDepth = newS2->nodeEnd->sideDepth;

            newS2->nodeEnd = end;

            statementNode *left = statementPos->left;

            if(globalScope->statementStart == statementPos)
              globalScope->statementStart = newNode;

            if(left)
              left->right = newNode;

            newNode->left = left;

            newNode->right     = statementPos;
            statementPos->left = newNode;

            ++(globalScope->statementCount);
          }
        }

        statementPos = statementPos->right;
      }
    }

    int parserBase::statementOccaForNest(statement &s){
      if( !(s.type != (forStatementType | occaStatementType)) )
        return notAnOccaFor;

      int ret = notAnOccaFor;

      const std::string &forName = s.nodeStart->value;

      if((forName.find("occaOuterFor") != std::string::npos) &&
         ((forName == "occaOuterFor0") ||
          (forName == "occaOuterFor1") ||
          (forName == "occaOuterFor2"))){

        ret = ((1 + forName[12] - '0') << occaOuterForShift);
      }
      else if((forName.find("occaInnerFor") != std::string::npos) &&
              ((forName == "occaInnerFor0") ||
               (forName == "occaInnerFor1") ||
               (forName == "occaInnerFor2"))){

        ret = ((1 + forName[12] - '0') << occaInnerForShift);
      }

      return ret;
    }

    bool parserBase::statementIsAnOccaFor(statement &s){
      const int nest = statementOccaForNest(s);

      return !(nest & notAnOccaFor);
    }

    void parserBase::fixOccaForStatementOrder(statement &origin,
                                              statementNode *sn){
      int innerLoopCount = -1;

      while(sn){
        std::vector<statement*> statStack;
        std::vector<int> nestStack;

        statement &s = *(sn->value);

        const int sNest = statementOccaForNest(s);

        if(sNest & notAnOccaFor){
          sn = sn->right;
          continue;
        }

        const bool isAnInnerLoop = (sNest & occaInnerForMask);

        const int shift = (isAnInnerLoop ? occaInnerForShift : occaOuterForShift);
        const int mask  = (isAnInnerLoop ? occaInnerForMask  : occaOuterForMask);

        statement *sp = &s;

        statStack.push_back(sp);
        nestStack.push_back((sNest >> shift) - 1);

        int loopCount = 1;

        sp = sp->statementStart->value;

        while(sp){
          const int nest = statementOccaForNest(*sp);

          if(nest & (~mask))
            break;

          statStack.push_back(sp);
          nestStack.push_back((nest >> shift) - 1);

          ++loopCount;

          sp = sp->statementStart->value;
        }

        if(isAnInnerLoop){
          if(innerLoopCount == -1){
            innerLoopCount = loopCount;
          }
          else{
            if(loopCount != innerLoopCount){
              std::cout << "Inner loops are inconsistent in:\n"
                        << origin << '\n';

              throw 1;
            }

            if(!statementHasBarrier( *(sn->left->value) )){
              std::cout << "Warning: Placing a local barrier between:\n"
                        << "---[ A ]--------------------------------\n"
                        << *(sn->left->value)
                        << "---[ B ]--------------------------------\n"
                        << *(sn->value)
                        << "========================================\n";

              statement *newS = new statement(sn->value->depth,
                                              declareStatementType, sn->value->up);

              statementNode *newSN = new statementNode(newS);

              newS->nodeStart = splitContent("occaBarrier(occaLocalMemFence);\0");
              newS->nodeStart = labelCode(newS->nodeStart);
              newS->nodeEnd   = lastNode(newS->nodeStart);

              if(sn->left)
                sn->left->right = newSN;

              newSN->left  = sn->left;
              newSN->right = sn;

              sn->left = newSN;
            }
          }
        }
        else{
          // Re-order inner loops
          fixOccaForStatementOrder(origin, (sp->up->statementStart));
        }

        std::sort(nestStack.begin(), nestStack.end());

        for(int i = (loopCount - 1); 0 <= i; --i){
          if(nestStack[i] != i){
            std::cout << "Inner loops ";

            for(int i2 = 0; i2 < loopCount; ++i2)
              std::cout << (i2 ? ", " : "[") << nestStack[i2];

            std::cout << "] have duplicates or gaps:\n"
                      << origin << '\n';

            throw 1;
          }

          sp = statStack[loopCount - i - 1];

          sp->nodeStart->value  = "occa";
          sp->nodeStart->value += (isAnInnerLoop ? "Inner" : "Outer");
          sp->nodeStart->value += "For0";

          sp->nodeStart->value[12] += i;
        }

        sn = sn->right;
      }
    }

    void parserBase::fixOccaForOrder(statement &s){
      if( !statementIsAKernel(s) )
        return;

      fixOccaForStatementOrder(s, s.statementStart);
    }

    void parserBase::addParallelFors(statement &s){
      if( !statementIsAKernel(s) )
        return;

      statementNode *snPos = s.statementStart;

      while(snPos){
        statement &s2 = *(snPos->value);

        const int nest = statementOccaForNest(s2);

        if(nest & (notAnOccaFor | occaInnerForMask)){

          snPos = snPos->right;
          continue;
        }

        const char outerDim = '0' + (nest - 1);

        statement *parallelStatement = new statement(s.depth + 1,
                                                     occaStatementType, &s);

        statementNode *parallelSN = new statementNode(parallelStatement);

        parallelStatement->nodeStart         = new strNode("occaParallelFor");
        parallelStatement->nodeStart->value += outerDim;
        parallelStatement->nodeStart->value += '\n';
        parallelStatement->type              = occaStatementType;

        if(s.statementStart == snPos)
          s.statementStart = parallelSN;

        statementNode *leftSN  = snPos->left;

        parallelSN->right = snPos;
        parallelSN->left  = leftSN;

        snPos->left = parallelSN->right;

        if(leftSN)
          leftSN->right = parallelSN;

        snPos = snPos->right;
      }
    }

    void parserBase::updateConstToConstant(statement &s){
      // Global scope only
      if((s.depth != 0) ||
         !(s.type & declareStatementType))
        return;

      strNode *nodePos = s.nodeStart;

      while(nodePos){
        if(nodePos->value == "occaConst")
          nodePos->value = "occaConstant";

        // [*] or [&]
        if(nodePos->type & (unknownVariable |
                            binaryOperatorType))
          break;

        nodePos = nodePos->right;
      }
    }

    strNode* parserBase::occaExclusiveStrNode(varInfo &info,
                                              const int depth,
                                              const int sideDepth){
      strNode *nodeRoot;

      const int typeInfo = info.typeInfo;

      if(typeInfo & pointerType)
        nodeRoot = new strNode("occaPrivateArray");
      else
        nodeRoot = new strNode("occaPrivate");

      nodeRoot->type      = presetValue;
      nodeRoot->depth     = depth;
      nodeRoot->sideDepth = sideDepth;

      strNode *nodePos = nodeRoot->pushDown("(");

      nodePos->type  = keywordType["("];
      nodePos->depth = depth + 1;

      const int descriptorCount = info.descriptors.size();

      for(int i = 0; i < descriptorCount; ++i){
        if(info.descriptors[i] == "exclusive")
          continue;

        nodePos       = nodePos->push(info.descriptors[i]);
        nodePos->type = qualifierType;
      }

      if(info.type){
        nodePos       = nodePos->push(info.type->typeName);
        nodePos->type = specifierType;

        if(typeInfo & heapPointerType){
          for(int i = 0; i < info.pointerCount; ++i){
            nodePos       = nodePos->push("*");
            nodePos->type = keywordType["*"];
          }
        }
      }

      nodePos       = nodePos->push(",");
      nodePos->type = keywordType[","];

      nodePos       = nodePos->push(info.name);
      nodePos->type = unknownVariable;

      if(typeInfo & stackPointerType){
        const int heapCount = info.stackPointerSizes.size();

        if(1 < heapCount){
          std::cout << "Only 1D exclusive variables are currently supported [" << info << "]\n";
          throw 1;
        }

        nodePos       = nodePos->push(",");
        nodePos->type = keywordType[","];

        nodePos       = nodePos->push(info.stackPointerSizes[0]);
        nodePos->type = presetValue;
      }

      nodePos       = nodePos->push(")");
      nodePos->type = keywordType[")"];

      nodePos       = nodePos->push(";");
      nodePos->type = keywordType[";"];

      return nodeRoot;
    }

    void parserBase::addKernelInfo(varInfo &info, statement &s){
      if( !(s.type & functionStatementType) ){
        node<strNode*> nNodeRoot = s.nodeStart->getStrNodesWith(info.name);

        node<strNode*> *nNodePos = nNodeRoot.right;

        while(nNodePos){
          strNode *nodePos = nNodePos->value;

          // If we're calling function, not using a function pointer
          if(nodePos->down.size()){
            nodePos = nodePos->down[0];

            if((nodePos->type == startParentheses) &&
               (nodePos->value != "occaKernelInfo")){
              strNode *kia = nodePos->push("occaKernelInfo");

              kia->up        = nodePos->up;
              kia->type      = keywordType["occaKernelInfo"];
              kia->depth     = nodePos->depth;
              kia->sideDepth = nodePos->sideDepth;

              if(kia->right->type != endParentheses){
                strNode *comma = kia->push(",");

                comma->up        = kia->up;
                comma->type      = keywordType[","];
                comma->depth     = kia->depth;
                comma->sideDepth = kia->sideDepth;
              }
            }
          }

          nNodePos = nNodePos->right;
        }
      }
    }

    void parserBase::addArgQualifiers(varInfo &info, statement &s){
      // Having functionCallType at this level means:
      //   occaExp, occaBarrier, etc
      // so don't add occaKernelInfoArg
      if((info.typeInfo & functionType) &&
         !(info.typeInfo & functionCallType)){
        strNode *nodePos = s.nodeStart;

        while(nodePos->down.size() == 0)
          nodePos = nodePos->right;

        nodePos = nodePos->down[0];

        if((nodePos->type == startParentheses) &&
           (nodePos->value != "occaKernelInfoArg")){
          strNode *kia = nodePos->push("occaKernelInfoArg");

          kia->up        = nodePos->up;
          kia->type      = keywordType["occaKernelInfoArg"];
          kia->depth     = nodePos->depth;
          kia->sideDepth = nodePos->sideDepth;

          strNode *comma = kia->push(",");

          comma->up        = kia->up;
          comma->type      = keywordType[","];
          comma->depth     = kia->depth;
          comma->sideDepth = kia->sideDepth;

          applyToStatementsUsingVar(info, &parserBase::addKernelInfo);

          while(nodePos){
            if((nodePos->value == ",") ||
               (nodePos->value == ")"))
              break;

            nodePos = nodePos->right;
          }
        }

        if(!info.hasDescriptor("occaKernel") &&
           !info.hasDescriptor("kernel"))
          return;

        while(nodePos){
          if(nodePos->value == ","){
            nodePos = nodePos->right;

            strNode *nextVar = nodePos;
            varInfo info = s.loadVarInfo(nextVar);

            if((info.typeInfo & pointerType)        &&
               (!info.hasDescriptor("occaPointer")) &&
               (!info.hasDescriptor("texture")) ){
              nodePos       = nodePos->push("occaPointer");
              nodePos->type = keywordType["occaPointer"];

              nodePos->swapWithLeft();
            }
            else if(!(info.typeInfo & pointerType) &&
                    !info.hasDescriptor("occaVariable") ){
              while( !(nodePos->type & unknownVariable) )
                nodePos = nodePos->right;

              nodePos       = nodePos->push("occaVariable");
              nodePos->type = keywordType["occaVariable"];

              nodePos->swapWithLeft();
            }

            nodePos = nextVar;
          }
          else
            nodePos = nodePos->right;
        }
      }
    }

    void parserBase::modifyExclusiveVariables(statement &s){
      if(!(s.type & declareStatementType))
        return;

      if(getStatementKernel(s) == NULL)
        return;

      if(statementKernelUsesNativeOCCA(s))
        return;

      strNode *nodePos = s.nodeStart;
      varInfo info     = s.loadVarInfo(nodePos);

      if(!info.hasDescriptor("exclusive"))
        return;

      statement *newS = new statement(s.depth,
                                      declareStatementType, s.up);

      newS->nodeStart = occaExclusiveStrNode(info,
                                             nodePos->depth,
                                             nodePos->sideDepth);

      newS->nodeEnd   = lastNode(newS->nodeStart);

      statementNode *newSN = new statementNode(newS);
      statementNode *upPos = (s.up)->statementStart;

      while(upPos->value != &s)
        upPos = upPos->right;

      statementNode *oldSN = upPos;

      if(upPos == (s.up)->statementStart){
        (s.up)->statementStart = newSN;

        if(oldSN->right)
          oldSN->right->left = newSN;

        newSN->left  = NULL;
        newSN->right = oldSN->right;
      }
      else{
        oldSN->left->right = newSN;

        if(oldSN->right)
          oldSN->right->left = newSN;

        newSN->left  = oldSN->left;
        newSN->right = oldSN->right;
      }

      while(nodePos){
        if(nodePos->value == ","){
          nodePos = nodePos->right;

          varInfo info2 = s.loadVarInfo(nodePos);

          info2.type        = info.type;
          info2.descriptors = info.descriptors;

          statement *newS2 = new statement(s.depth,
                                           declareStatementType, s.up);

          newS2->nodeStart = occaExclusiveStrNode(info2,
                                                  nodePos->depth,
                                                  nodePos->sideDepth);

          newS2->nodeEnd   = lastNode(newS2->nodeStart);

          newSN = newSN->push(newS2);
        }
        else
          nodePos = nodePos->right;
      }

      // [-] Needs proper free (can't because it's nested...)
      //   delete oldSN;
    }

    void parserBase::modifyStatementOccaForVariables(varInfo &var,
                                                     strNode *n){
      const int extras = var.extraInfo.size();

      while(n){
        if((n->type & unknownVariable) &&
           (n->value == var.name)){

          if(extras == 1)
            n->value = var.extraInfo[0];
          else
            n->value = ("(" + (var.extraInfo[0] +
                               " + " +
                               var.extraInfo[1]) +
                        ")");
        }

        const int downCount = n->down.size();

        for(int i = 0; i < downCount; ++i)
          modifyStatementOccaForVariables(var, n->down[i]);

        n = n->right;
      }
    }

    void parserBase::modifyOccaForVariables(){
      varUsedMapIterator it = varUsedMap.begin();

      while(it != varUsedMap.end()){
        varInfo *infoPtr = it->first;
        const int extras = infoPtr->extraInfo.size();

        // Only occa id/dim's have extras atm
        if(extras){
          // First node is just a placeholder
          statementNode *sNodePos = (it->second).right;

          while(sNodePos){
            statement &s = *(sNodePos->value);

            modifyStatementOccaForVariables(*infoPtr,
                                            s.nodeStart);

            sNodePos = sNodePos->right;
          }
        }

        ++it;
      }
    }

    void parserBase::modifyTextureVariables(){
      varUsedMapIterator it = varUsedMap.begin();

      while(it != varUsedMap.end()){
        varInfo *infoPtr = it->first;

        if(infoPtr->hasDescriptor("texture")){
          statement &os = *(varOriginMap[infoPtr]);

          strNode *osNodePos = os.nodeStart;

          while(osNodePos){
            if(osNodePos->value == infoPtr->name)
              std::cout << "HERE!\n";

            osNodePos = osNodePos->right;
          }

          // First node is just a placeholder
          statementNode *sNodePos = (it->second).right;

          while(sNodePos){
            statement &s = *(sNodePos->value);
            strNode *nodePos = s.nodeStart;

            while(nodePos){
              if((nodePos->type & unknownVariable) &&
                 (nodePos->value == infoPtr->name)){
                // [-] HERE
                std::cout
                  << "2. nodePos = " << *nodePos << '\n';
              }

              nodePos = nodePos->right;
            }

            sNodePos = sNodePos->right;
          }
        }

        ++it;
      }
    }

    statementNode* parserBase::splitKernelStatement(statementNode *sn,
                                                    kernelInfo &info){
      int kernelCount = 0;

      statement &s = *(sn->value);
      statementNode *snPos = s.statementStart;

      while(snPos){
        statement &s2 = *(snPos->value);

        if( (s2.type != (forStatementType | occaStatementType)) &&
            !(s2.type & macroStatementType) ){

          std::cout << "Only outer-loops are supported at the kernel scope\n";
          throw 1;
        }

        if(s2.type == (forStatementType | occaStatementType))
          ++kernelCount;

        snPos = snPos->right;
      }

      if(kernelCount == 1)
        return sn->right;

      std::stringstream ss;

      while(true){
        int k;

        for(k = 0; k < kernelCount; ++k){
          ss << k;

          if(globalScope->hasVariableInScope(info.baseName + ss.str()))
            break;

          ss.str("");
        }

        if(k == kernelCount)
          break;

        info.baseName += '_';
      }

      statementNode *newSNRoot, *newSNEnd;

      info.nestedKernels.clear();

      scopeVarMapIterator it = globalScope->scopeVarMap.find(info.name);
      varInfo &originalVar   = *(it->second);

      globalScope->scopeVarMap.erase(it);

      for(int k = 0; k < kernelCount; ++k){
        strNode *newNodeRoot = s.nodeStart->clone();
        strNode *newNodeEnd  = lastNode(newNodeRoot);

        info.nestedKernels.push_back(new statement(s.depth,
                                                   s.type, globalScope));

        while( !(newNodeRoot->type & unknownVariable) )
          newNodeRoot = newNodeRoot->right;

        ss << k;
        newNodeRoot->value = info.baseName + ss.str();
        originalVar.name   = newNodeRoot->value;
        ss.str("");

        globalScope->addVariable(originalVar);

        if(k)
          newSNEnd = newSNEnd->push(new statementNode(info.nestedKernels.back()));
        else
          newSNRoot = newSNEnd = new statementNode(info.nestedKernels.back());
      }

      if(globalScope->statementStart == sn)
        globalScope->statementStart = newSNRoot;

      if(sn->left)
        sn->left->right = newSNRoot;

      if(sn->right)
        sn->right->left = newSNEnd;

      newSNRoot->left = sn->left;
      newSNEnd->right = sn->right;

      statementNode *snPosStart = s.statementStart;
      snPos = snPosStart;

      kernelCount = 0;

      // Add kernel bodies
      while(snPos){
        statement &s2 = *(snPos->value);

        if((s2.type == (forStatementType | occaStatementType)) ||
           (snPos->right == NULL)){

          statement &s3 = *(info.nestedKernels[kernelCount]);

          if( !(s2.type == (forStatementType | occaStatementType)) )
            snPos = NULL;

          s3.statementStart = snPosStart;
          s3.statementEnd   = snPos;

          if(snPosStart)
            snPosStart->left = NULL;

          if(snPos){
            statementNode *snPosRight = snPos->right;
            snPos->right = NULL;
            snPos        = snPosRight;

            snPosStart = snPosRight;
          }

          ++kernelCount;
        }
        else
          snPos = snPos->right;

        globalScope->statementCount += (kernelCount - 1);
      }

      // [-] Free sn and s

      return newSNEnd->right;
    }

    void parserBase::loadKernelInfos(){
      statementNode *snPos = globalScope->statementStart;

      while(snPos){
        statement &s = *(snPos->value);

        if(statementIsAKernel(s)){
          //---[ Setup Info ]-----------
          kernelInfo &info = *(new kernelInfo);

          strNode *nodePos = s.nodeStart;

          while( !(nodePos->type & unknownVariable) )
            nodePos = nodePos->right;

          info.name     = nodePos->value;
          info.baseName = nodePos->value;
          info.nestedKernels.push_back(&s);

          kernelInfoMap[info.name] = &info;
          //============================

          snPos = splitKernelStatement(snPos, info);
        }
        else
          snPos = snPos->right;
      }
    }

    std::string parserBase::occaScope(statement &s){
      statement *currentS = &s;

      while(currentS){
        if(currentS->type == (forStatementType | occaStatementType))
          break;

        currentS = currentS->up;
      }

      if(currentS == NULL)
        return "";

      return s.nodeStart->value;
    }

    void parserBase::incrementDepth(statement &s){
      ++s.depth;
    }

    void parserBase::decrementDepth(statement &s){
      --s.depth;
    }

    bool parserBase::statementHasBarrier(statement &s){
      strNode *n = s.nodeStart;

      while(n){
        if(n->value == "occaBarrier")
          return true;

        n = n->right;
      }

      return false;
    }

    statementNode* parserBase::findStatementWith(statement &s,
                                                 findStatementWith_t func){
      statementNode *ret = new statementNode(&s);

      if((this->*func)(s))
        return ret;

      statementNode *statementPos = s.statementStart;

      int found = 0;

      while(statementPos){
        statementNode *ret2 = findStatementWith(*(statementPos->value), func);

        if(ret2 != NULL){
          ret->pushDown(ret2);
          ++found;
        }

        statementPos = statementPos->right;
      }

      if(found)
        return ret;

      delete ret;

      return NULL;
    }

    int parserBase::getKernelOuterDim(statement &s){
      statementNode *statementPos = s.statementStart;

      std::string outerStr = obfuscate("Outer");
      int outerDim = -1;

      varInfo *info = s.hasVariableInScope(outerStr);

      if(info != NULL){
        const int extras = info->extraInfo.size();

        for(int i = 0; i < extras; ++i){
          const int loopNest = (info->extraInfo[i][0] - '0');

          if(outerDim < loopNest)
            outerDim = loopNest;

          // Max Dim
          if(outerDim == 2)
            return outerDim;
        }
      }

      while(statementPos){
        const int outerDim2 = getKernelOuterDim( *(statementPos->value) );

        if(outerDim < outerDim2)
          outerDim = outerDim2;

        // Max Dim
        if(outerDim == 2)
          return outerDim;

        statementPos = statementPos->right;
      }

      return outerDim;
    }

    int parserBase::getKernelInnerDim(statement &s){
      statementNode *statementPos = s.statementStart;

      std::string innerStr = obfuscate("Inner");
      int innerDim = -1;

      varInfo *info = s.hasVariableInScope(innerStr);

      if(info != NULL){
        const int extras = info->extraInfo.size();

        for(int i = 0; i < extras; ++i){
          const int loopNest = (info->extraInfo[i][0] - '0');

          if(innerDim < loopNest)
            innerDim = loopNest;

          // Max Dim
          if(innerDim == 2)
            return innerDim;
        }
      }

      while(statementPos){
        const int innerDim2 = getKernelInnerDim( *(statementPos->value) );

        if(innerDim < innerDim2)
          innerDim = innerDim2;

        // Max Dim
        if(innerDim == 2)
          return innerDim;

        statementPos = statementPos->right;
      }

      return innerDim;
    }

    void parserBase::checkPathForConditionals(statementNode *path){
      if((path == NULL) ||
         (path->value == NULL))
        return;

      if(path->value->type & ifStatementType){
        std::cout << '\n'
                  << "+---------+--------------------------------------------------+\n"
                  << "|         | Barriers inside conditional statements will only |\n"
                  << "| Warning | work properly if it's always called or never     |\n"
                  << "|         | called.                                          |\n"
                  << "+---------+--------------------------------------------------+\n"
                  << *(path->value)
                  << "+---------+--------------------------------------------------+\n\n";
      }

      const int downCount = path->down.size();

      for(int i = 0; i < downCount; ++i)
        checkPathForConditionals(path->down[i]);
    }

    int parserBase::findLoopSections(statement &s,
                                     statementNode *path,
                                     loopSection_t &loopSection,
                                     int section){
      if(s.statementCount == 0)
        return section;

      int downCount = 0;
      int downPos   = 0;

      statementNode *sPos = s.statementStart;
      statement *sNext = NULL;

      if(path != NULL){
        downCount = path->down.size();

        if(downCount)
          sNext = path->down[0]->value;
      }

      while(sPos){
        if(sPos->value == sNext){
          // Last one is a barrier
          if(path->down[downPos]->down.size() == 0)
            ++section;

          section = findLoopSections(*(sPos->value),
                                     path->down[downPos],
                                     loopSection,
                                     section);

          if(++downPos < downCount)
            sNext = path->down[downPos]->value;
          else
            sNext = NULL;
        }
        else{
          loopSection[sPos->value] = section;

          findLoopSections(*(sPos->value),
                           NULL,
                           loopSection,
                           section);
        }

        sPos = sPos->right;
      }

      return section;
    }

    bool parserBase::varInTwoSegments(varInfo &info,
                                      loopSection_t &loopSection){
      // Don't count shared memory
      if(info.hasDescriptor("occaPointer")  ||
         info.hasDescriptor("occaVariable") ||
         info.hasDescriptor("occaShared"))
        return false;

      varUsedMapIterator it = varUsedMap.find(&info);

      // Variable is not used
      if(it == varUsedMap.end())
        return false;

      statementNode *pos = (it->second).right;

      const int segment = loopSection[pos->value];
      pos = pos->right;

      while(pos){
        if(segment != loopSection[pos->value])
          return true;

        pos = pos->right;
      }

      return false;
    }

    varInfoNode* parserBase::findVarsMovingToTop(statement &s,
                                                 loopSection_t &loopSection){
      // Statement defines have doubles (to know how many variables
      //                                 were defined)
      //    so ... ignore duplicates
      const bool ignoringVariables = (s.type & simpleStatementType);

      scopeVarMapIterator it = s.scopeVarMap.begin();
      varInfoNode *root = NULL;
      varInfoNode *pos  = NULL;

      if(!ignoringVariables){
        while(it != s.scopeVarMap.end()){
          varInfo &info = *(it->second);

          if(varInTwoSegments(info, loopSection)){
            // No longer const
            info.removeDescriptor("occaConst");

            if(root == NULL){
              root = new varInfoNode(&info);
              pos  = root;
            }
            else
              pos = pos->push(&info);
          }

          ++it;
        }
      }

      statementNode *sn = s.statementStart;

      while(sn){
        varInfoNode *pos2 = findVarsMovingToTop(*(sn->value),
                                                loopSection);

        if(pos2 != NULL){
          pos->right = pos2;
          pos2->left = pos;

          pos = lastNode(pos);
        }

        sn = sn->right;
      }

      return root;
    }

    void parserBase::splitDefineForVariable(statement *&origin,
                                            varInfo &var, strNode *varNode,
                                            const int declPos){
      const int declarationCount = origin->scopeVarMap.size();
      const bool addingStatement = ((declPos != 0) || (declarationCount > 1));

      statement &originUp  = *(origin->up);
      statementNode *snPos = NULL;

      bool removeVarStatement = true;
      bool ignoringFirst      = false;
      bool ignoringSecond     = false;

      snPos = originUp.statementStart;

      while(snPos->value != origin)
        snPos = snPos->right;

      // If it's something like
      //   const int [a = 0], b = 0 ...
      // stitch
      //   const int b = 0 ...
      // and paste
      //   a = 0;
      if(varNode->right->value == "="){
        removeVarStatement = false;

        if(addingStatement)
          varNode = varNode->left;
      }

      if(declPos == 0){
        // const int [a = 0];
        if(declarationCount == 1){
          while(origin->nodeStart != varNode)
            popAndGoRight(origin->nodeStart);

          origin->type = updateStatementType;
        }
        // const int [a = 0], ...;
        else{
          ignoringSecond = true;

          // Removing const int [* const] a = NULL
          while(varNode->type != specifierType)
            popAndGoLeft(varNode);

          varNode = varNode->right;

          if(removeVarStatement){
            ignoringFirst = true;

            while(varNode->value != ",")
              popAndGoRight(varNode);
          }
          else{
            while(varNode->value != ",")
              varNode = varNode->right;
          }

          // Remove the right [,]
          popAndGoRight(varNode);
        }
      }
      // const int a = 0, [b = 0], ...;
      else{
        while(varNode->value != ",")
          popAndGoLeft(varNode);

        if(removeVarStatement){
          while((varNode->value != ",") &&
                (varNode->value != ";"))
            popAndGoRight(varNode);
        }
        else{
          while((varNode->value != ",") &&
                (varNode->value != ";"))
            varNode = varNode->right;
        }

        // Remove the right [,]
        popAndGoRight(varNode);
      }

      origin->scopeVarMap.erase(var.name);

      var.descriptors.insert(var.descriptors.begin(), "exclusive");
      strNode *newVarNode = var.makeStrNodeChain();

      statement *newS = new statement(origin->depth,
                                      declareStatementType,
                                      origin->up);

      varOriginMap[&var]          = newS;
      newS->scopeVarMap[var.name] = &var;

      statementNode *newVarSN = new statementNode(newS);

      statement *rootStatement = origin;

      while( !(rootStatement->type & (forStatementType |
                                      functionStatementType)) )
        rootStatement = rootStatement->up;

      statementNode *oldFirstPos = rootStatement->statementStart;

      rootStatement->statementStart = newVarSN;
      newVarSN->right               = oldFirstPos;
      oldFirstPos->left             = newVarSN;

      newS->depth = rootStatement->depth + 1;
      newS->up    = rootStatement;

      if(!addingStatement){
        if((declarationCount == 1) && removeVarStatement){
          if(originUp.statementStart == snPos)
            originUp.statementStart = snPos->right;

          if(snPos->left)
            snPos->left->right = snPos->right;

          if(snPos->right)
            snPos->right->left = snPos->left;
        }

        return;
      }

      varUsedMap[&var].push(origin);

      strNode *firstNodeStart = firstNode(varNode);
      strNode *firstNodeEnd    = varNode->left;
      strNode *secondNodeStart = varNode;

      secondNodeStart->left = NULL;

      if(firstNodeEnd){
        firstNodeEnd->right = NULL;
        firstNodeEnd        = firstNodeEnd->push(";");
        firstNodeEnd->type  = keywordType[";"];

        // Seal off first define
        origin->nodeEnd = firstNodeEnd;
      }

      strNode *secondNodeEnd = secondNodeStart;

      if(ignoringSecond){
        while(secondNodeEnd->value != ";")
          secondNodeEnd = secondNodeEnd->right;
      }
      else{
        while((secondNodeEnd->value != ",") &&
              (secondNodeEnd->value != ";"))
          secondNodeEnd = secondNodeEnd->right;
      }

      secondNodeEnd->value = ";";
      secondNodeEnd->type  = keywordType[";"];

      // Create second update
      if(!ignoringSecond &&
         !removeVarStatement){
        statement *secondS = new statement(origin->depth,
                                           updateStatementType,
                                           origin->up);

        snPos = snPos->push(secondS);
        varUsedMap[&var].push(secondS);
      }

      // Create third define
      strNode *thirdNodeStart = ignoringSecond ? secondNodeStart : secondNodeEnd->right;
      strNode *thirdNodeEnd   = lastNode(thirdNodeStart);

      if(thirdNodeStart){
        secondNodeEnd->right = NULL;
        thirdNodeStart->left = NULL;

        // Copy over the desciptors to the next statement
        strNode *thirdPrefix = firstNodeStart->clone();

        if( !(thirdPrefix->type & specifierType) ){
          strNode *firstNodePos = firstNodeStart->right;

          while( !(firstNodePos->type & specifierType) ){
            thirdPrefix = thirdPrefix->push( firstNodePos->clone() );
            firstNodePos = firstNodePos->right;
          }
        }

        thirdPrefix->right   = thirdNodeStart;
        thirdNodeStart->left = thirdPrefix;

        thirdNodeStart = firstNode(thirdNodeStart);

        statement *thirdS;

        if(!ignoringFirst){
          thirdS = new statement(origin->depth,
                                 declareStatementType,
                                 origin->up);

          snPos = snPos->push(thirdS);
        }
        else
          thirdS = origin;

        thirdNodeEnd = thirdNodeStart;

        while(thirdNodeEnd){
          if(thirdNodeEnd->type & unknownVariable){
            scopeVarMapIterator it = origin->scopeVarMap.find(thirdNodeEnd->value);
            varInfo &movingVar = *(it->second);

            origin->scopeVarMap.erase(it);

            varOriginMap[&movingVar]            = thirdS;
            thirdS->scopeVarMap[movingVar.name] = &movingVar;

            while((thirdNodeEnd->value != ",") &&
                  (thirdNodeEnd->value != ";"))
              thirdNodeEnd = thirdNodeEnd->right;
          }

          thirdNodeEnd = thirdNodeEnd->right;
        }
      }

      if(!ignoringFirst){
        // Gotta remove the descriptors
        if(ignoringSecond){
          while( !(firstNodeStart->type & unknownVariable) )
            popAndGoRight(firstNodeStart);
        }

        origin->nodeStart = firstNodeStart;
        origin->nodeEnd   = firstNodeEnd;
      }
      else{
        origin->nodeStart = thirdNodeStart;
        origin->nodeEnd   = thirdNodeEnd;
      }

      if((declarationCount == 1) && removeVarStatement){
        if(originUp.statementStart == snPos)
          originUp.statementStart = snPos->right;

        if(snPos->left)
          snPos->left->right = snPos->right;

        if(snPos->right)
          snPos->right->left = snPos->left;
      }
    }

    void parserBase::addInnerForsToStatement(statement &s,
                                             const int innerDim){
      statementNode *sn = s.statementStart;

      while(sn &&
            (sn->value->hasDescriptorVariable("occaShared") ||
             sn->value->hasDescriptorVariable("exclusive")  ||
             statementHasBarrier( *(sn->value) )))
        sn = sn->right;

      while(sn){
        sn = addInnerForsBetweenBarriers(s, sn, innerDim);

        while(sn &&
              statementHasBarrier( *(sn->value) ))
          sn = sn->right;
      }
    }

    statementNode* parserBase::addInnerForsBetweenBarriers(statement &origin,
                                                           statementNode *includeStart,
                                                           const int innerDim){
      if(includeStart == NULL)
        return NULL;

      const int occaForType = keywordType["occaInnerFor0"];

      statement *outerMostLoop = NULL;
      statement *innerMostLoop = NULL;

      statementNode *includeEnd = includeStart;
      statementNode *returnNode;

      bool stoppedAtFor = false;

      while(includeEnd){
        if(statementHasBarrier( *(includeEnd->value) ))
          break;

        if(includeEnd->value->type & forStatementType){
          stoppedAtFor = true;
          break;
        }

        includeEnd = includeEnd->right;
      }

      returnNode = (includeEnd ? includeEnd->right : NULL);

      if(includeEnd && stoppedAtFor){
        statementNode *forStart = includeEnd;
        statementNode *forNode  = includeEnd;

        while(forStart->left &&
              forStart->left->value->type & macroStatementType)
          forStart = forStart->left;

        includeEnd = forStart;

        addInnerForsToStatement(*(forNode->value), innerDim);

        if(includeStart == forStart)
          return returnNode;
      }

      if(includeEnd)
        includeEnd = includeEnd->left;

      for(int i = innerDim; 0 <= i; --i){
        statement *newStatement = new statement(includeStart->value->depth,
                                                occaForType, &origin);

        newStatement->nodeStart = new strNode("occaInnerFor");
        newStatement->nodeStart->value += '0' + i;
        newStatement->nodeStart->type   = occaForType;

        newStatement->nodeEnd = newStatement->nodeStart;

        if(i == innerDim){
          outerMostLoop = newStatement;
          innerMostLoop = outerMostLoop;
        }
        else{
          innerMostLoop->addStatement(newStatement);
          innerMostLoop->statementStart->value->up = innerMostLoop;
          innerMostLoop = innerMostLoop->statementStart->value;
        }
      }

      // Keep privates and shared outside inner loops
      while(includeStart &&
            (includeStart->value->hasDescriptorVariable("occaShared") ||
             includeStart->value->hasDescriptorVariable("exclusive"))){

        includeStart = includeStart->right;
      }

      // Put the loop node on the origin's statements
      //   and remove nodes that are going to be put
      //   in the inner loops
      statementNode *outerMostLoopSN = new statementNode(outerMostLoop);

      if(origin.statementStart == includeStart)
        origin.statementStart = outerMostLoopSN;

      outerMostLoopSN->left = includeStart->left;

      if(includeStart->left)
        includeStart->left->right = outerMostLoopSN;

      includeStart->left = NULL;

      if(includeEnd){
        outerMostLoopSN->right = includeEnd->right;

        if(includeEnd->right)
          includeEnd->right->left = outerMostLoopSN;

        includeEnd->right = NULL;
      }

      innerMostLoop->statementStart = includeStart;

      while(includeStart != includeEnd){
        includeStart->value->up = innerMostLoop;
        includeStart = includeStart->right;
      }

      if(includeEnd)
        includeStart->value->up = innerMostLoop;

      // Increment the depth of statements in the loops
      for(int i = 0; i < innerDim; ++i){
        outerMostLoop = outerMostLoop->statementStart->value;
        applyToAllStatements(*outerMostLoop, &parserBase::incrementDepth);
      }

      applyToAllStatements(*outerMostLoop, &parserBase::incrementDepth);
      --(outerMostLoop->depth);

      return returnNode;
    }

    void parserBase::addInnerFors(statement &s){
      int innerDim = getKernelInnerDim(s);

      if(innerDim == -1){
        std::cout << "OCCA Inner for-loop count could not be calculated\n";
        throw 1;
      }

      // Get path and ignore kernel
      statementNode *sPath = findStatementWith(s, &parserBase::statementHasBarrier);

      checkPathForConditionals(sPath);

      loopSection_t loopSection;
      findLoopSections(s, sPath, loopSection);

      // Get private and shared vars
      varInfoNode *varRoot = findVarsMovingToTop(s, loopSection);
      varInfoNode *varPos  = lastNode(varRoot);

      statementNode *newStatementStart = NULL;
      statementNode *newStatementPos   = NULL;

      while(varPos){
        statement *origin = (varOriginMap[varPos->value]);

        // Ignore kernel arguments
        if(origin->type & functionStatementType){
          varPos = varPos->left;
          continue;
        }

        varInfo &info = *(varPos->value);

        strNode *nodePos = origin->nodeStart;
        int declPos = 0;

        while(nodePos){
          if(nodePos->type & unknownVariable){
            if(nodePos->value == info.name)
              break;

            ++declPos;
          }

          nodePos = nodePos->right;
        }

        splitDefineForVariable(origin,
                               info, nodePos,
                               declPos);

        varPos = varPos->left;
      }

      addInnerForsToStatement(s, innerDim);
    }

    void parserBase::addOuterFors(statement &s){
      int outerDim = getKernelOuterDim(s);

      if(outerDim == -1){
        std::cout << "OCCA Outer for-loop count could not be calculated\n";
        throw 1;
      }

      const int occaForType = keywordType["occaOuterFor0"];

      statement *sPos = &s;

      for(int o = outerDim; 0 <= o; --o){
        statement *newStatement = new statement(sPos->depth + 1,
                                                occaForType, &s);

        newStatement->nodeStart = new strNode("occaOuterFor");
        newStatement->nodeStart->value += '0' + o;
        newStatement->nodeStart->type   = occaForType;

        newStatement->nodeEnd = newStatement->nodeStart;

        newStatement->scopeVarMap = sPos->scopeVarMap;

        statementNode *sn = sPos->statementStart;

        while(sn){
          newStatement->addStatement(sn->value);

          sn->value->up = newStatement;
          applyToAllStatements(*(sn->value), &parserBase::incrementDepth);

          statementNode *sn2 = sn->right;
          delete sn;
          sn = sn2;
        }

        sPos->statementCount = 0;
        sPos->statementStart = sPos->statementEnd = NULL;
        sPos->scopeVarMap.clear();

        sPos->addStatement(newStatement);

        sPos = newStatement;
      }
    }

    void parserBase::removeUnnecessaryBlocksInKernel(statement &s){
      statement *sPos = &s;

      // Get rid of empty blocks
      //  kernel void blah(){{  -->  kernel void blah(){
      //  }}                    -->  }
      while(sPos->statementCount == 1){
        statement *sDown = sPos->statementStart->value;

        if(sDown->type == blockStatementType){
          sPos->scopeVarMap.insert(sDown->scopeVarMap.begin(),
                                   sDown->scopeVarMap.end());

          sPos->statementCount = 0;
          sPos->statementStart = sPos->statementEnd = NULL;

          statementNode *sn = sDown->statementStart;

          while(sn){
            sPos->addStatement(sn->value);

            sn->value->up = sPos;
            applyToAllStatements(*(sn->value), &parserBase::decrementDepth);

            statementNode *sn2 = sn->right;
            delete sn;
            sn = sn2;
          }
        }
        else
          break;
      }
    }

    void parserBase::floatSharedVarsInKernel(statement &s){
      statementNode *sn = s.statementStart;

      statementNode *sharedStart = NULL;
      statementNode *sharedPos   = NULL;

      while(sn){
        statementNode *sn2 = sn;
        statement &s2      = *(sn->value);

        sn = sn->right;

        if((s2.type & declareStatementType) &&
           (s2.hasDescriptorVariable("occaShared") ||
            s2.hasDescriptorVariable("exclusive"))){

          if(s.statementStart == sn2)
            s.statementStart = sn;

          if(sn2->left)
            sn2->left->right = sn2->right;
          if(sn2->right)
            sn2->right->left = sn2->left;

          sn2->left  = NULL;
          sn2->right = NULL;

          if(sharedStart){
            sharedPos->right = sn2;
            sn2->left        = sharedPos;
            sharedPos        = sn2;
          }
          else{
            sharedStart = sn2;
            sharedPos   = sharedStart;
          }
        }
      }

      if(sharedStart){
        if(s.statementStart == NULL)
          s.statementStart = sharedStart;
        else{
          statementNode *oldStart = s.statementStart;

          s.statementStart = sharedStart;
          sharedPos->right = oldStart;
          oldStart->left   = sharedPos;
        }
      }
    }

    void parserBase::addOccaForsToKernel(statement &s){
      addInnerFors(s);
      addOuterFors(s);
    }

    void parserBase::addOccaFors(){
      statementNode *statementPos = globalScope->statementStart;

      while(statementPos){
        statement *s = statementPos->value;

        if(statementIsAKernel(*s)            && // Kernel
           (s->statementStart != NULL)       && //   not empty
           !statementKernelUsesNativeOKL(*s) && //   not OKL
           !statementKernelUsesNativeOCCA(*s)){ //   not OCCA

          removeUnnecessaryBlocksInKernel(*s);
          floatSharedVarsInKernel(*s);
          addOccaForsToKernel(*s);
        }

        statementPos = statementPos->right;
      }
    }

    void parserBase::setupOccaVariables(statement &s){
      const int idKeywordType = keywordType["occaInnerId0"];

      strNode *nodePos = s.nodeStart;

      while(nodePos){
        if(nodePos->type & idKeywordType){
          bool isInnerId = ((nodePos->value == "occaInnerId0") ||
                            (nodePos->value == "occaInnerId1") ||
                            (nodePos->value == "occaInnerId2"));

          bool isOuterId = ((nodePos->value == "occaOuterId0") ||
                            (nodePos->value == "occaOuterId1") ||
                            (nodePos->value == "occaOuterId2"));

          bool isInnerDim = ((nodePos->value == "occaInnerDim0") ||
                             (nodePos->value == "occaInnerDim1") ||
                             (nodePos->value == "occaInnerDim2"));

          bool isOuterDim = ((nodePos->value == "occaOuterDim0") ||
                             (nodePos->value == "occaOuterDim1") ||
                             (nodePos->value == "occaOuterDim2"));

          if(isInnerId  || isOuterId ||
             isInnerDim || isOuterDim){
            std::string ioLoop, loopNest;

            if(isInnerId || isOuterId){
              // [occa][-----][Id#]
              ioLoop = nodePos->value.substr(4,5);
              // [occa][-----Id][#]
              loopNest = nodePos->value.substr(11,1);
            }
            else{
              // [occa][-----][Dim#]
              ioLoop = nodePos->value.substr(4,5);
              // [occa][-----Dim][#]
              loopNest = nodePos->value.substr(12,1);
            }

            addOccaForCounter(s, ioLoop, loopNest);
          }
        }

        nodePos = nodePos->right;
      }
    }
    //==============================================

    strNode* splitContent(const std::string &str){
      return splitContent(str.c_str());
    }

    strNode* splitContent(const char *cRoot){
      initKeywords();

      const char *c = cRoot;

      strNode *nodeRoot = new strNode();
      strNode *nodePos  = nodeRoot;

      int status = readingCode;

      int lineCount = 0;

      while(*c != '\0'){
        const char *cEnd = readLine(c);

        std::string line = strip(c, cEnd - c);

        if(line.size()){
          if(status != insideCommentBlock){
            status = stripComments(line);
            strip(line);

            if(line.size()){
              nodePos->originalLine = lineCount;
              nodePos = nodePos->push(line);
            }
          }
          else{
            status = stripComments(line);
            strip(line);

            if((status == finishedCommentBlock) && line.size()){
              nodePos->originalLine = lineCount;
              nodePos = nodePos->push(line);
            }
          }
        }

        c = cEnd;
        ++lineCount;
      }

      popAndGoRight(nodeRoot);

      return nodeRoot;
    }

    strNode* labelCode(strNode *lineNodeRoot){
      initKeywords();

      strNode *nodeRoot = new strNode();
      strNode *nodePos  = nodeRoot;

      strNode *lineNodePos = lineNodeRoot;

      int depth = 0;

      while(lineNodePos){
        const std::string &line = lineNodePos->value;
        const char *cLeft = line.c_str();

        while(*cLeft != '\0'){
          skipWhitespace(cLeft);

          const char *cRight = cLeft;

          bool loadString = isAString(cLeft);
          bool loadNumber = isANumber(cLeft);

          // Case: n +1
          if(loadNumber){
            const int delimeterChars = isAWordDelimeter(cLeft);

            if((delimeterChars == 1) &&
               ((cLeft[0] == '+') || (cLeft[0] == '-'))){

              if(nodePos->left)
                loadNumber = false;
            }
          }

          if(loadString){
            skipString(cRight);

            nodePos = nodePos->push( std::string(cLeft, (cRight - cLeft)) );

            nodePos->type  = presetValue;
            nodePos->depth = depth;

            cLeft = cRight;
          }
          else if(loadNumber){
            const int delimeterChars = isAWordDelimeter(cLeft);

            skipNumber(cRight);

            nodePos = nodePos->push( std::string(cLeft, (cRight - cLeft)) );

            nodePos->type  = presetValue;
            nodePos->depth = depth;

            cLeft = cRight;
          }
          else{
            const int delimeterChars = isAWordDelimeter(cLeft);

            if(delimeterChars){
              strNode *newNode = new strNode(std::string(cLeft, delimeterChars));

              newNode->type = keywordType[newNode->value];

              newNode->depth = depth;

              if(newNode->type & startSection){
                ++depth;

                nodePos = nodePos->pushDown(newNode);
              }
              else if(newNode->type & endSection){
                nodePos = nodePos->push(newNode);

                --depth;
                nodePos = nodePos->up;
              }
              else if(newNode->type & macroKeywordType){
                newNode->value = line;
                nodePos = nodePos->push(newNode);

                cLeft = line.c_str() + strlen(line.c_str()) - delimeterChars;
              }
              else
                nodePos = nodePos->push(newNode);

              cLeft += delimeterChars;
            }
            else{
              skipWord(cRight);

              nodePos = nodePos->push( std::string(cLeft, (cRight - cLeft)) );

              keywordTypeMapIterator it = keywordType.find(nodePos->value);

              if(it == keywordType.end())
                nodePos->type = unknownVariable;
              else{
                nodePos->type = it->second;

                // Merge [else] [if] -> [else if]
                if((nodePos->type & flowControlType)       &&
                   (nodePos->left)                         &&
                   (nodePos->left->type & flowControlType) &&
                   ((nodePos->value == "if")         &&
                    (nodePos->left->value == "else") &&
                    (nodePos->left->down.size() == 0))){

                  nodePos->value = "else if";

                  strNode *elseNode = nodePos->left;

                  nodePos->left        = nodePos->left->left;
                  nodePos->left->right = nodePos;

                  delete elseNode->pop();
                }
              }

              nodePos->depth = depth;

              cLeft = cRight;
            }
          }
        }

        lineNodePos = lineNodePos->right;
      }

      if((nodePos != nodeRoot) &&
         (nodeRoot->down.size() == 0))
        popAndGoRight(nodeRoot);

      free(lineNodeRoot);

      return nodeRoot;
    }

    void initKeywords(){
      if(keywordsAreInitialized)
        return;

      keywordsAreInitialized = true;

      //---[ Operator Info ]--------------
      keywordType["!"]  = lUnitaryOperatorType;
      keywordType["%"]  = binaryOperatorType;
      keywordType["&"]  = (binaryOperatorType | qualifierType);
      keywordType["("]  = startParentheses;
      keywordType[")"]  = endParentheses;
      keywordType["*"]  = (binaryOperatorType | qualifierType);
      keywordType["+"]  = (lUnitaryOperatorType | binaryOperatorType);
      keywordType[","]  = binaryOperatorType;
      keywordType["-"]  = (lUnitaryOperatorType | binaryOperatorType);
      keywordType["."]  = binaryOperatorType;
      keywordType["/"]  = binaryOperatorType;
      keywordType[":"]  = endStatement;
      keywordType[";"]  = endStatement;
      keywordType["<"]  = binaryOperatorType;
      keywordType["="]  = binaryOperatorType;
      keywordType[">"]  = binaryOperatorType;
      keywordType["?"]  = ternaryOperatorType;
      keywordType["["]  = startBracket;
      keywordType["]"]  = endBracket;
      keywordType["^"]  = (binaryOperatorType | qualifierType);
      keywordType["{"]  = startBrace;
      keywordType["|"]  = binaryOperatorType;
      keywordType["}"]  = endBrace;
      keywordType["~"]  = lUnitaryOperatorType;
      keywordType["!="] = assOperatorType;
      keywordType["%="] = assOperatorType;
      keywordType["&&"] = binaryOperatorType;
      keywordType["&="] = assOperatorType;
      keywordType["*="] = assOperatorType;
      keywordType["+="] = assOperatorType;
      keywordType["++"] = unitaryOperatorType;
      keywordType["-="] = assOperatorType;
      keywordType["--"] = unitaryOperatorType;
      keywordType["->"] = binaryOperatorType;
      keywordType["/="] = assOperatorType;
      keywordType["::"] = binaryOperatorType;
      keywordType["<<"] = binaryOperatorType;
      keywordType["<="] = binaryOperatorType;
      keywordType["=="] = binaryOperatorType;
      keywordType[">="] = binaryOperatorType;
      keywordType[">>"] = binaryOperatorType;
      keywordType["^="] = assOperatorType;
      keywordType["|="] = assOperatorType;
      keywordType["||"] = binaryOperatorType;

      keywordType["#"] = macroKeywordType;

      //---[ Types & Specifiers ]---------
      std::string suffix[6] = {"", "2", "3", "4", "8", "16"};

      for(int i = 0; i < 6; ++i){
        keywordType[std::string("int")    + suffix[i]] = specifierType;
        keywordType[std::string("bool")   + suffix[i]] = specifierType;
        keywordType[std::string("char")   + suffix[i]] = specifierType;
        keywordType[std::string("long")   + suffix[i]] = specifierType;
        keywordType[std::string("short")  + suffix[i]] = specifierType;
        keywordType[std::string("float")  + suffix[i]] = specifierType;
        keywordType[std::string("double") + suffix[i]] = specifierType;
      }

      keywordType["void"]          = specifierType;
      keywordType["__attribute__"] = specifierType; // [--]

      keywordType["long"]     = (qualifierType | specifierType);
      keywordType["short"]    = (qualifierType | specifierType);
      keywordType["signed"]   = qualifierType;
      keywordType["unsigned"] = qualifierType;

      keywordType["inline"] = qualifierType;
      keywordType["static"] = qualifierType;
      keywordType["extern"] = qualifierType;

      keywordType["const"]    = (qualifierType | occaKeywordType);
      keywordType["restrict"] = (qualifierType | occaKeywordType);
      keywordType["volatile"] = (qualifierType | occaKeywordType);
      keywordType["aligned"]  = (qualifierType | occaKeywordType);
      keywordType["register"] = qualifierType;

      keywordType["occaConst"]    = (qualifierType | occaKeywordType);
      keywordType["occaRestrict"] = (qualifierType | occaKeywordType);
      keywordType["occaVolatile"] = (qualifierType | occaKeywordType);
      keywordType["occaAligned"]  = (qualifierType | occaKeywordType);
      keywordType["occaConstant"] = (qualifierType | occaKeywordType);

      keywordType["enum"]    = (structType);
      keywordType["class"]   = (structType);
      keywordType["union"]   = (structType);
      keywordType["struct"]  = (structType | qualifierType);
      keywordType["typedef"] = (typedefType);

      //---[ C++ ]----------------------
      keywordType["virtual"]   = qualifierType;

      keywordType["namespace"] = (specifierType | structType);

      //---[ Constants ]------------------
      keywordType["..."]   = presetValue;
      keywordType["true"]  = presetValue;
      keywordType["false"] = presetValue;

      //---[ Flow Control ]---------------
      keywordType["if"]   = flowControlType;
      keywordType["else"] = flowControlType;

      keywordType["for"] = flowControlType;

      keywordType["do"]    = flowControlType;
      keywordType["while"] = flowControlType;

      keywordType["switch"]  = flowControlType;
      keywordType["case"]    = specialKeywordType;
      keywordType["default"] = specialKeywordType;

      keywordType["break"]    = specialKeywordType;
      keywordType["continue"] = specialKeywordType;
      keywordType["return"]   = specialKeywordType;
      keywordType["goto"]     = specialKeywordType;

      //---[ OCCA Keywords ]--------------
      keywordType["kernel"]    = (qualifierType | occaKeywordType);
      keywordType["texture"]   = (qualifierType | occaKeywordType);
      keywordType["shared"]    = (qualifierType | occaKeywordType);
      keywordType["exclusive"] = (qualifierType | occaKeywordType);

      keywordType["occaKernel"]   = (qualifierType | occaKeywordType);
      keywordType["occaFunction"] = (qualifierType | occaKeywordType);
      keywordType["occaPointer"]  = (qualifierType | occaKeywordType);
      keywordType["occaVariable"] = (qualifierType | occaKeywordType);
      keywordType["occaShared"]   = (qualifierType | occaKeywordType);

      keywordType["occaKernelInfoArg"] = (presetValue | occaKeywordType);
      keywordType["occaKernelInfo"]    = (presetValue | occaKeywordType);

      keywordType["barrier"]        = (presetValue | occaKeywordType);
      keywordType["localMemFence"]  = (presetValue | occaKeywordType);
      keywordType["globalMemFence"] = (presetValue | occaKeywordType);

      keywordType["occaBarrier"]        = (presetValue | occaKeywordType);
      keywordType["occaLocalMemFence"]  = (presetValue | occaKeywordType);
      keywordType["occaGlobalMemFence"] = (presetValue | occaKeywordType);

      keywordType["occaInnerFor0"] = (forStatementType | occaStatementType);
      keywordType["occaInnerFor1"] = (forStatementType | occaStatementType);
      keywordType["occaInnerFor2"] = (forStatementType | occaStatementType);

      keywordType["occaOuterFor0"] = (forStatementType | occaStatementType);
      keywordType["occaOuterFor1"] = (forStatementType | occaStatementType);
      keywordType["occaOuterFor2"] = (forStatementType | occaStatementType);

      keywordType["occaInnerId0"] = (presetValue | occaKeywordType);
      keywordType["occaInnerId1"] = (presetValue | occaKeywordType);
      keywordType["occaInnerId2"] = (presetValue | occaKeywordType);

      keywordType["occaOuterId0"] = (presetValue | occaKeywordType);
      keywordType["occaOuterId1"] = (presetValue | occaKeywordType);
      keywordType["occaOuterId2"] = (presetValue | occaKeywordType);

      keywordType["occaGlobalId0"] = (presetValue | occaKeywordType);
      keywordType["occaGlobalId1"] = (presetValue | occaKeywordType);
      keywordType["occaGlobalId2"] = (presetValue | occaKeywordType);

      keywordType["occaInnerDim0"] = (presetValue | occaKeywordType);
      keywordType["occaInnerDim1"] = (presetValue | occaKeywordType);
      keywordType["occaInnerDim2"] = (presetValue | occaKeywordType);

      keywordType["occaOuterDim0"] = (presetValue | occaKeywordType);
      keywordType["occaOuterDim1"] = (presetValue | occaKeywordType);
      keywordType["occaOuterDim2"] = (presetValue | occaKeywordType);

      keywordType["occaGlobalDim0"] = (presetValue | occaKeywordType);
      keywordType["occaGlobalDim1"] = (presetValue | occaKeywordType);
      keywordType["occaGlobalDim2"] = (presetValue | occaKeywordType);

      //---[ CUDA Keywords ]--------------
      keywordType["threadIdx"] = (unknownVariable | cudaKeywordType);
      keywordType["blockDim"]  = (unknownVariable | cudaKeywordType);
      keywordType["blockIdx"]  = (unknownVariable | cudaKeywordType);
      keywordType["gridDim"]   = (unknownVariable | cudaKeywordType);

      std::string mathFunctions[16] = {
        "sqrt", "sin"  , "asin" ,
        "sinh", "asinh", "cos"  ,
        "acos", "cosh" , "acosh",
        "tan" , "atan" , "tanh" ,
        "atanh", "exp" , "log2" ,
        "log10"
      };

      for(int i = 0; i < 16; ++i){
        std::string mf = mathFunctions[i];
        std::string cmf = mf;
        cmf[0] += ('A' - 'a');

        keywordType["occa"       + cmf] = presetValue;
        keywordType["occaFast"   + cmf] = presetValue;
        keywordType["occaNative" + cmf] = presetValue;
      }

      //---[ Operator Precedence ]--------
      opPrecedence[opHolder("::", binaryOperatorType)]   = 0;

      // class(...), class{1,2,3}, static_cast<>(), func(), arr[]
      opPrecedence[opHolder("++", rUnitaryOperatorType)] = 1;
      opPrecedence[opHolder("--", rUnitaryOperatorType)] = 1;
      opPrecedence[opHolder("." , binaryOperatorType)]   = 1;
      opPrecedence[opHolder("->", binaryOperatorType)]   = 1;

      // (int) x, sizeof, new, new [], delete, delete []
      opPrecedence[opHolder("++", lUnitaryOperatorType)] = 2;
      opPrecedence[opHolder("--", lUnitaryOperatorType)] = 2;
      opPrecedence[opHolder("+" , lUnitaryOperatorType)] = 2;
      opPrecedence[opHolder("-" , lUnitaryOperatorType)] = 2;
      opPrecedence[opHolder("!" , lUnitaryOperatorType)] = 2;
      opPrecedence[opHolder("~" , lUnitaryOperatorType)] = 2;
      opPrecedence[opHolder("*" , qualifierType)]        = 2;
      opPrecedence[opHolder("&" , qualifierType)]        = 2;

      opPrecedence[opHolder(".*" , binaryOperatorType)]  = 3;
      opPrecedence[opHolder("->*", binaryOperatorType)]  = 3;

      opPrecedence[opHolder("*" , binaryOperatorType)]   = 4;
      opPrecedence[opHolder("/" , binaryOperatorType)]   = 4;
      opPrecedence[opHolder("%" , binaryOperatorType)]   = 4;

      opPrecedence[opHolder("+" , binaryOperatorType)]   = 5;
      opPrecedence[opHolder("-" , binaryOperatorType)]   = 5;

      opPrecedence[opHolder("<<", binaryOperatorType)]   = 6;
      opPrecedence[opHolder(">>", binaryOperatorType)]   = 6;

      opPrecedence[opHolder("<" , binaryOperatorType)]   = 7;
      opPrecedence[opHolder("<=", binaryOperatorType)]   = 7;
      opPrecedence[opHolder(">=", binaryOperatorType)]   = 7;
      opPrecedence[opHolder(">" , binaryOperatorType)]   = 7;

      opPrecedence[opHolder("==", binaryOperatorType)]   = 8;
      opPrecedence[opHolder("!=", binaryOperatorType)]   = 8;

      opPrecedence[opHolder("&" , binaryOperatorType)]   = 9;

      opPrecedence[opHolder("^" , binaryOperatorType)]   = 10;

      opPrecedence[opHolder("|" , binaryOperatorType)]   = 11;

      opPrecedence[opHolder("&&", binaryOperatorType)]   = 12;

      opPrecedence[opHolder("||", binaryOperatorType)]   = 13;

      opPrecedence[opHolder("?" , ternaryOperatorType)]  = 14;
      opPrecedence[opHolder("=" , assOperatorType)]      = 14;
      opPrecedence[opHolder("+=", assOperatorType)]      = 14;
      opPrecedence[opHolder("-=", assOperatorType)]      = 14;
      opPrecedence[opHolder("*=", assOperatorType)]      = 14;
      opPrecedence[opHolder("/=", assOperatorType)]      = 14;
      opPrecedence[opHolder("%=", assOperatorType)]      = 14;
      opPrecedence[opHolder("<<=", assOperatorType)]     = 14;
      opPrecedence[opHolder(">>=", assOperatorType)]     = 14;
      opPrecedence[opHolder("&=", assOperatorType)]      = 14;
      opPrecedence[opHolder("^=", assOperatorType)]      = 14;
      opPrecedence[opHolder("|=", assOperatorType)]      = 14;

      // 15: throw x

      opPrecedence[opHolder("," , binaryOperatorType)]   = 16;

      opLevelMap[ 0]["::"]  = binaryOperatorType;
      opLevelMap[ 1]["++"]  = rUnitaryOperatorType;
      opLevelMap[ 1]["--"]  = rUnitaryOperatorType;
      opLevelMap[ 1]["." ]  = binaryOperatorType;
      opLevelMap[ 1]["->"]  = binaryOperatorType;
      opLevelMap[ 2]["++"]  = lUnitaryOperatorType;
      opLevelMap[ 2]["--"]  = lUnitaryOperatorType;
      opLevelMap[ 2]["+" ]  = lUnitaryOperatorType;
      opLevelMap[ 2]["-" ]  = lUnitaryOperatorType;
      opLevelMap[ 2]["!" ]  = lUnitaryOperatorType;
      opLevelMap[ 2]["~" ]  = lUnitaryOperatorType;
      opLevelMap[ 2]["*" ]  = qualifierType;
      opLevelMap[ 2]["&" ]  = qualifierType;
      opLevelMap[ 3][".*" ] = binaryOperatorType;
      opLevelMap[ 3]["->*"] = binaryOperatorType;
      opLevelMap[ 4]["*" ]  = binaryOperatorType;
      opLevelMap[ 4]["/" ]  = binaryOperatorType;
      opLevelMap[ 4]["%" ]  = binaryOperatorType;
      opLevelMap[ 5]["+" ]  = binaryOperatorType;
      opLevelMap[ 5]["-" ]  = binaryOperatorType;
      opLevelMap[ 6]["<<"]  = binaryOperatorType;
      opLevelMap[ 6][">>"]  = binaryOperatorType;
      opLevelMap[ 7]["<" ]  = binaryOperatorType;
      opLevelMap[ 7]["<="]  = binaryOperatorType;
      opLevelMap[ 7][">="]  = binaryOperatorType;
      opLevelMap[ 7][">" ]  = binaryOperatorType;
      opLevelMap[ 8]["=="]  = binaryOperatorType;
      opLevelMap[ 8]["!="]  = binaryOperatorType;
      opLevelMap[ 9]["&" ]  = binaryOperatorType;
      opLevelMap[10]["^" ]  = binaryOperatorType;
      opLevelMap[11]["|" ]  = binaryOperatorType;
      opLevelMap[12]["&&"]  = binaryOperatorType;
      opLevelMap[13]["||"]  = binaryOperatorType;
      opLevelMap[14]["?" ]  = ternaryOperatorType;
      opLevelMap[14]["=" ]  = assOperatorType;
      opLevelMap[14]["+="]  = assOperatorType;
      opLevelMap[14]["-="]  = assOperatorType;
      opLevelMap[14]["*="]  = assOperatorType;
      opLevelMap[14]["/="]  = assOperatorType;
      opLevelMap[14]["%="]  = assOperatorType;
      opLevelMap[14]["<<="] = assOperatorType;
      opLevelMap[14][">>="] = assOperatorType;
      opLevelMap[14]["&="]  = assOperatorType;
      opLevelMap[14]["^="]  = assOperatorType;
      opLevelMap[14]["|="]  = assOperatorType;
      opLevelMap[16][","]   = binaryOperatorType;

      /*---[ Future Ones ]----------------
        keywordType["using"] = ;
        keywordType["namespace"] = ;
        keywordType["template"] = ;
        ================================*/
    }
  };
};
