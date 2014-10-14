#include "occaParserStatement.hpp"
#include "occaParser.hpp"

namespace occa {
  namespace parserNamespace {
    //---[ Statement Functions ]--------------------
    statement::statement(parserBase &pb) :
      depth(-1),
      type(blockStatementType),

      up(NULL),

      varOriginMap(pb.varOriginMap),
      varUsedMap(pb.varUsedMap),

      expCount(0),
      exp(NULL),

      nodeStart(NULL),
      nodeEnd(NULL),

      statementCount(0),
      statementStart(NULL),
      statementEnd(NULL) {}

    statement::statement(const int depth_,
                         const int type_,
                         statement *up_,
                         strNode *nodeStart_, strNode *nodeEnd_) :
      depth(depth_),
      type(type_),

      up(up_),

      varOriginMap(up_->varOriginMap),
      varUsedMap(up_->varUsedMap),

      expCount(0),
      exp(NULL),

      nodeStart(nodeStart_),
      nodeEnd(nodeEnd_),

      statementCount(0),
      statementStart(NULL),
      statementEnd(NULL) {}

    statement::~statement(){};

    std::string statement::getTab() const {
      std::string ret = "";

      for(int i = 0; i < depth; ++i)
        ret += "  ";

      return ret;
    }

    int statement::statementType(strNode *&nodeRoot){
      if(nodeRoot == NULL)
        return invalidStatementType;

      else if(nodeRoot->type == macroKeywordType)
        return checkMacroStatementType(nodeRoot);

      else if(nodeRoot->type == keywordType["occaOuterFor0"])
        return checkOccaForStatementType(nodeRoot);

      else if(nodeRoot->type & structType)
        return checkStructStatementType(nodeRoot);

      else if(nodeRoot->type & operatorType)
        return checkUpdateStatementType(nodeRoot);

      else if(nodeHasDescriptor(nodeRoot))
        return checkDescriptorStatementType(nodeRoot);

      else if(nodeRoot->type & unknownVariable){
        if(nodeRoot->right &&
           nodeRoot->right->value == ":")
          return checkGotoStatementType(nodeRoot);

        return checkUpdateStatementType(nodeRoot);
      }

      else if(nodeRoot->type & flowControlType)
        return checkFlowStatementType(nodeRoot);

      else if(nodeRoot->type & specialKeywordType)
        return checkSpecialStatementType(nodeRoot);

      else if((nodeRoot->type == startBrace) &&
              (nodeRoot->up)                 &&
              !(nodeRoot->up->type & operatorType))
        return checkBlockStatementType(nodeRoot);

      while(nodeRoot &&
            !(nodeRoot->type & endStatement))
        nodeRoot = nodeRoot->right;

      return declareStatementType;
    }

    int statement::checkMacroStatementType(strNode *&nodeRoot){
      return macroStatementType;
    }

    int statement::checkOccaForStatementType(strNode *&nodeRoot){
      return keywordType["occaOuterFor0"];
    }

    int statement::checkStructStatementType(strNode *&nodeRoot){
      if((nodeRoot->value == "struct")       &&
         (nodeRoot->down.size() == 0)        &&
         (nodeRoot->right)                   &&
         (nodeRoot->right->down.size() == 0)){

        if(hasTypeInScope(nodeRoot->right->value))
          return checkDescriptorStatementType(nodeRoot);
      }

      while(nodeRoot){
        if(nodeRoot->type & endStatement)
          break;

        nodeRoot = nodeRoot->right;
      }

      return structStatementType;
    }

    int statement::checkUpdateStatementType(strNode *&nodeRoot){
      while(nodeRoot){
        if(nodeRoot->type & endStatement)
          break;

        nodeRoot = nodeRoot->right;
      }

      return updateStatementType;
    }

    int statement::checkDescriptorStatementType(strNode *&nodeRoot){
#if 1
      strNode *oldNodeRoot = nodeRoot;
      strNode *nodePos;

      // Skip descriptors
      while((nodeRoot)                         &&
            (nodeRoot->down.size() == 0)       &&
            ((nodeRoot->type & descriptorType) ||
             nodeHasSpecifier(nodeRoot))){

        nodeRoot = nodeRoot->right;
      }

      nodePos = nodeRoot;

      while((nodeRoot) &&
            !(nodeRoot->type & endStatement))
        nodeRoot = nodeRoot->right;

      const int downCount = nodePos->down.size();

      // Function {Proto, Def | Ptr}
      if(downCount && (nodePos->down[0]->type & parentheses)){
        if(downCount == 1)
          return functionPrototypeType;

        strNode *downNode = nodePos->down[1];

        if(downNode->type & brace){
          nodeRoot = nodePos;
          return functionDefinitionType;
        }

        else if(downNode->type & parentheses){
          downNode = downNode->right;

          if(downNode->type & parentheses){
            std::cout << "Function pointer needs a [*]:\n"
                      << prettyString(oldNodeRoot, "  ");
            throw 1;
          }

          while(downNode->value == "*"){
            if(downNode->down.size()){
              if(nodeRoot == NULL){
                std::cout << "Missing a [;] after:\n"
                          << prettyString(oldNodeRoot, "  ");
                throw 1;
              }

              return declareStatementType;
            }

            downNode = downNode->right;
          }

          if(nodeRoot == NULL){
            std::cout << "Missing a [;] after:\n"
                      << prettyString(oldNodeRoot, "  ");
            throw 1;
          }

          // [C++] Function call, not function pointer define
          //    getFunc(0)(arg1, arg2);
          if(hasVariableInScope(downNode->value))
            return updateStatementType;

          return declareStatementType;
        }
        else{
          std::cout << "You found the [Waldo 1] error in:\n"
                    << prettyString(oldNodeRoot, "  ");
          throw 1;
        }
      }

      if(nodeRoot == NULL){
        std::cout << "Missing a [;] after:\n"
                  << prettyString(oldNodeRoot, "  ");
        throw 1;
      }

      return declareStatementType;
#else
      while(nodeRoot){
        if(nodeRoot->type & endStatement)
          return declareStatementType;

        // Case:
        //   const varName = ();
        if(nodeRoot->type & operatorType){
          while(nodeRoot){
            if(nodeRoot->type & endStatement)
              break;

            nodeRoot = nodeRoot->right;
          }

          return declareStatementType;
        }

        else if(nodeRoot->down.size() &&
                (nodeRoot->down[0]->type & parentheses)){

          const int downCount = nodeRoot->down.size();

          if(downCount == 1){
            while(nodeRoot){
              if(nodeRoot->type & endStatement)
                break;

              if(nodeRoot->right == NULL){
                std::cout << "Missing a [;] after:\n"
                          << prettyString(nodeRoot, "  ");
                throw 1;
              }

              nodeRoot = nodeRoot->right;
            }

            return functionPrototypeType;
          }
          else
            return functionDefinitionType;
        }

        if(nodeRoot->right == NULL)
          break;

        nodeRoot = nodeRoot->right;
      }

      return declareStatementType;
#endif
    }

    int statement::checkGotoStatementType(strNode *&nodeRoot){
      nodeRoot = nodeRoot->right;
      return gotoStatementType;
    }

    int statement::checkFlowStatementType(strNode *&nodeRoot){
      if(nodeRoot->value == "for")
        return forStatementType;
      else if(nodeRoot->value == "while")
        return whileStatementType;
      else if(nodeRoot->value == "do")
        return doWhileStatementType;
      else if(nodeRoot->value == "if")
        return ifStatementType;
      else if(nodeRoot->value == "else if")
        return elseIfStatementType;
      else if(nodeRoot->value == "else")
        return elseStatementType;
      else if(nodeRoot->value == "switch")
        return switchStatementType;

      std::cout << "You found the [Waldo 2] error in:\n"
                << prettyString(nodeRoot, "  ");
      throw 1;

      return 0;
    }

    int statement::checkSpecialStatementType(strNode *&nodeRoot){
      while(nodeRoot){
        if(nodeRoot->type & endStatement)
          break;

        nodeRoot = nodeRoot->right;
      }

      return blankStatementType;
    }

    int statement::checkBlockStatementType(strNode *&nodeRoot){
      nodeRoot = lastNode(nodeRoot);

      return blockStatementType;
    }

    void statement::addTypeDef(const std::string &typeDefName){
      typeDef &def = *(new typeDef);
      def.typeName = typeDefName;
      scopeTypeMap[typeDefName] = &def;
    }

    bool statement::nodeHasQualifier(strNode *n) const {
      if( !(n->type & qualifierType) )
        return false;

      // short and long can be both:
      //    specifiers and qualifiers
      if( !(n->type & specifierType) )
        return true;

      if((n->right) == NULL)
        return false;

      return (n->right->type & descriptorType);
    }

    bool statement::nodeHasSpecifier(strNode *n) const {
      return ((n->type & specifierType) ||
              ((n->type & unknownVariable) &&
               ( hasTypeInScope(n->value) )));
    }

    bool statement::nodeHasDescriptor(strNode *n) const {
      if(nodeHasSpecifier(n) || nodeHasQualifier(n))
        return true;

      return false;
    }

    varInfo statement::loadVarInfo(strNode *&nodePos){
      varInfo info;

#if 0
      while(nodePos &&
            ((nodePos->type & qualifierType) ||
             nodeHasSpecifier(nodePos))){

        if(nodePos->type & structType){
          info.descriptors.push_back(*nodePos);
          info.typeInfo |= structType;
        }

        else if(nodeHasQualifier(nodePos)){
          if(nodePos->value == "*"){
            info.typeInfo |= heapPointerType;
            ++info.pointerCount;

            if(nodePos->right &&
               nodePos->right->value == "const"){
              info.typeInfo |= constPointerType;
              nodePos = nodePos->right;
            }
          }
          else if(nodePos->value == "&")
            info.typeInfo |= referenceType;
          else{
            if((nodePos->value  == "texture")              ||
               ((nodePos->value == "image1d_t")            ||
                (nodePos->value == "image2d_t"))           ||
               (nodePos->value  == "cudaSurfaceObject_t"))
              info.typeInfo |= textureType;

            info.descriptors.push_back(nodePos->value);
          }
        }

        else if(nodeHasSpecifier(nodePos)){
          info.type = hasTypeInScope(*nodePos);

          if(info.type == NULL){
            std::cout << "Type [" << *nodePos << "] is not defined.\nFound in:\n";
            nodePos->print();
            throw 1;
          }
        }

        if(nodePos->down.size() != 0)
          break;

        nodePos = nodePos->right;
      }

      if((nodePos == NULL) ||
         (nodePos->type & (endSection |
                           endStatement))){

        if(nodePos &&
           nodePos->value == ":"){

          info.bitField = atoi(nodePos->right->value.c_str());
          nodePos = nodePos->right->right;
        }

        return info;
      }

      const int downCount = nodePos->down.size();

      if(downCount == 0){
        info.name      = nodePos->value;
        info.typeInfo |= variableType;

        nodePos = nodePos->right;

        if(nodePos &&
           nodePos->value == ":"){

          info.bitField = atoi(nodePos->right->value.c_str());
          nodePos = nodePos->right->right;

          return info;
        }
      }
      else{
        strNode *downNode = nodePos->down[0];

        if(downNode->type == startParentheses){
          // [-] Only for C
          if((2 <= downCount) &&
             (nodePos->down[1]->type == startParentheses)){

            downNode = downNode->right;

            while(downNode->value == "*"){
              ++(info.pointerCount);
              downNode = downNode->right;
            }

            info.name      = downNode->value;
            info.typeInfo |= functionPointerType;

            if(downNode->down.size()){
              const int downCount2 = downNode->down.size();

              for(int i = 0; i < downCount2; ++i){
                if(downNode->down[i]->type != startBracket)
                  break;

                std::string sps = prettyString(downNode->down[i]);
                sps = sps.substr(1, sps.size() - 2); // Remove '[' and ']'

                info.stackPointerSizes.push_back(sps);
              }
            }

            downNode = nodePos->down[1]->right;

            while(downNode){
              varInfo &arg = *(new varInfo);
              arg = loadVarInfo(downNode);

              info.vars.push_back(&arg);

              // Loaded last arg
              if((downNode == NULL) ||
                 (downNode->right == NULL))
                break;

              downNode = downNode->right;
            }

            nodePos = nodePos->right;
          }
          else{
            info.name      = nodePos->value;
            info.typeInfo |= functionType;

            downNode = downNode->right;

            while(downNode){
              varInfo &arg = *(new varInfo);
              arg = loadVarInfo(downNode);

              info.vars.push_back(&arg);

              downNode = downNode->right;

              // Loaded last arg
              if(downNode == NULL)
                break;
            }

            // Distinguish between prototypes and function calls
            if(nodePos->right &&
               (nodePos->right->value == ";")){
              if(info.type)
                info.typeInfo |= protoType;
              else
                info.typeInfo |= functionCallType;
            }
          }
        }
        else if(downNode->type == startBracket){
          info.name      = nodePos->value;
          info.typeInfo |= (variableType | stackPointerType);

          for(int i = 0; i < downCount; ++i){
            if(nodePos->down[i]->type != startBracket)
              break;

            std::string sps = prettyString(nodePos->down[i]);
            sps = sps.substr(1, sps.size() - 2); // Remove '[' and ']'

            info.stackPointerSizes.push_back(sps);
          }
        }
      }

      return info;
#else
      while(nodePos                         &&
            !(nodePos->type & presetValue)  &&
            !(nodePos->type & endStatement) && // For bitfields
            (!(nodePos->type & unknownVariable) ||
             hasTypeInScope(nodePos->value))){

        if(nodePos->type & structType){
          info.descriptors.push_back(*nodePos);
          info.typeInfo |= structType;
        }

        else if(nodeHasQualifier(nodePos)){
          if(nodePos->value == "*"){
            info.typeInfo |= heapPointerType;
            ++info.pointerCount;

            if(nodePos->right &&
               nodePos->right->value == "const"){
              info.typeInfo |= constPointerType;
              nodePos = nodePos->right;
            }
          }
          else if(nodePos->value == "&")
            info.typeInfo |= referenceType;
          else{
            if((nodePos->value  == "texture")              ||
               ((nodePos->value == "image1d_t")            ||
                (nodePos->value == "image2d_t"))           ||
               (nodePos->value  == "cudaSurfaceObject_t"))
              info.typeInfo |= textureType;

            info.descriptors.push_back(*nodePos);
          }
        }

        else if(nodeHasSpecifier(nodePos)){
          info.type = hasTypeInScope(*nodePos);

          if(info.type == NULL){
            std::cout << "Type [" << *nodePos << "] is not defined.\nFound in:\n";
            nodePos->print();
            throw 1;
          }
        }

        nodePos = nodePos->right;
      }

      if((nodePos == NULL) ||
         (nodePos->type & endStatement)) // For bitfields
        return info;

      info.name = *nodePos;

      const int downCount = nodePos->down.size();

      if(downCount){
        if(nodePos->down[0]->type == startParentheses){
          strNode *argPos = nodePos->down[0];
          info.typeInfo |= functionType;

          strNode *lastPos = lastNode(nodePos);

          // Distinguish between prototypes and function calls
          if(lastPos->value == ";"){
            if(info.type)
              info.typeInfo |= protoType;
            else
              info.typeInfo |= functionCallType;
          }
        }
        else if(nodePos->down[0]->type == startBracket){
          info.typeInfo |= (variableType | stackPointerType);

          for(int i = 0; i < downCount; ++i){
            nodePos->down[i]->type = startBracket;

            std::string sps = prettyString(nodePos->down[i]);
            sps = sps.substr(1, sps.size() - 2); // Remove '[' and ']'

            info.stackPointerSizes.push_back(sps);
          }
        }
      }
      else{
        info.typeInfo |= variableType;
        nodePos = nodePos->right;
      }

      return info;
#endif
    }

    typeDef* statement::hasTypeInScope(const std::string &typeName) const {
      cScopeTypeMapIterator it = scopeTypeMap.find(typeName);

      if(it != scopeTypeMap.end())
        return it->second;

      if(up)
        return up->hasTypeInScope(typeName);

      return NULL;
    }

    varInfo* statement::hasVariableInScope(const std::string &varName) const {
      cScopeVarMapIterator it = scopeVarMap.find(varName);

      if(it != scopeVarMap.end())
        return it->second;

      if(up)
        return up->hasVariableInScope(varName);

      return NULL;
    }

    bool statement::hasDescriptorVariable(const std::string descriptor) const {
      cScopeVarMapIterator it = scopeVarMap.begin();

      while(it != scopeVarMap.end()){
        if((it->second)->hasDescriptor(descriptor))
          return true;

        ++it;
      }

      return false;
    }

    bool statement::hasDescriptorVariableInScope(const std::string descriptor) const {
      if(hasDescriptorVariable(descriptor))
        return true;

      if(up != NULL)
        return up->hasDescriptorVariable(descriptor);

      return false;
    }

    void statement::loadAllFromNode(strNode *nodeRoot){
      while(nodeRoot)
        nodeRoot = loadFromNode(nodeRoot);
    }

    strNode* statement::loadFromNode(strNode *nodeRoot){
      strNode *nodeRootEnd = nodeRoot;

      // Finds statement type and sets nodeRootEnd to the
      //    last strNode in that statement
      const int st = statementType(nodeRootEnd);

      if(st & invalidStatementType){
        std::cout << "Not a valid statement\n";
        throw 1;
      }

      statement *newStatement = new statement(depth + 1,
                                              st, this,
                                              nodeRoot, nodeRootEnd);

      if( !(st & ifStatementType) )
        addStatement(newStatement);

      if(st & simpleStatementType){
        nodeRootEnd = newStatement->loadSimpleFromNode(st,
                                                       nodeRoot,
                                                       nodeRootEnd);
      }

      else if(st & flowStatementType){
        if(st & forStatementType)
          nodeRootEnd = newStatement->loadForFromNode(st,
                                                      nodeRoot,
                                                      nodeRootEnd);

        else if(st & whileStatementType)
          nodeRootEnd = newStatement->loadWhileFromNode(st,
                                                        nodeRoot,
                                                        nodeRootEnd);

        else if(st & ifStatementType){
          delete newStatement;

          nodeRootEnd = loadIfFromNode(st,
                                       nodeRoot,
                                       nodeRootEnd);
        }

        else if(st & switchStatementType)
          nodeRootEnd = newStatement->loadSwitchFromNode(st,
                                                         nodeRoot,
                                                         nodeRootEnd);

        else if(st & gotoStatementType)
          nodeRootEnd = newStatement->loadGotoFromNode(st,
                                                       nodeRoot,
                                                       nodeRootEnd);
      }
      else if(st & functionStatementType){
        if(st & functionDefinitionType)
          nodeRootEnd = newStatement->loadFunctionDefinitionFromNode(st,
                                                                     nodeRoot,
                                                                     nodeRootEnd);

        else if(st & functionPrototypeType)
          nodeRootEnd = newStatement->loadFunctionPrototypeFromNode(st,
                                                                    nodeRoot,
                                                                    nodeRootEnd);
      }
      else if(st & blockStatementType)
        nodeRootEnd = newStatement->loadBlockFromNode(st,
                                                      nodeRoot,
                                                      nodeRootEnd);

      else if(st & structStatementType)
        nodeRootEnd = newStatement->loadStructFromNode(st,
                                                       nodeRoot,
                                                       nodeRootEnd);

      else if(st & blankStatementType)
        nodeRootEnd = newStatement->loadBlankFromNode(st,
                                                      nodeRoot,
                                                      nodeRootEnd);

      else if(st & macroStatementType)
        nodeRootEnd = newStatement->loadMacroFromNode(st,
                                                      nodeRoot,
                                                      nodeRootEnd);

      return nodeRootEnd;
    }

    void statement::loadBlocksFromLastNode(strNode *end,
                                           const int startBlockPos){
      if(end == NULL)
        return;

      const int downCount = end->down.size();

      if(startBlockPos <= downCount){
        for(int i = startBlockPos; i < downCount; ++i)
          up->loadAllFromNode(end->down[i]);

        end->down.erase(end->down.begin() + startBlockPos,
                        end->down.end());
      }
    }

    strNode* statement::loadSimpleFromNode(const int st,
                                           strNode *nodeRoot,
                                           strNode *nodeRootEnd){
      strNode *nextNode = nodeRootEnd ? nodeRootEnd->right : NULL;

      if(nodeRoot)
        nodeRoot->left = NULL;
      if(nodeRootEnd)
        nodeRootEnd->right = NULL;

      loadBlocksFromLastNode(nodeRootEnd);

      return nextNode;
    }

    strNode* statement::loadForFromNode(const int st,
                                        strNode *nodeRoot,
                                        strNode *nodeRootEnd){
      strNode *nextNode = nodeRootEnd ? nodeRootEnd->right : NULL;

      if(nodeRoot)
        nodeRoot->left = NULL;
      if(nodeRootEnd)
        nodeRootEnd->right = NULL;

      const int downCount = nodeRootEnd->down.size();

      if(downCount == 1){
        // for(;;)
        //   stuff;
        if(nodeRoot->down[0]->type == startParentheses)
          return loadFromNode(nextNode);

        // occaOuterFor {
        // }
        else{
          strNode *blockStart = nodeRootEnd->down[0];
          strNode *blockEnd   = lastNode(blockStart);

          nodeRootEnd->down.erase(nodeRootEnd->down.begin() + 0,
                                  nodeRootEnd->down.begin() + 1);

          // Load all down's before popping [{] and [}]'s
          const int blockDownCount = blockStart->down.size();

          for(int i = 0; i < blockDownCount; ++i)
            loadAllFromNode( blockStart->down[i] );

          popAndGoRight(blockStart);
          popAndGoLeft(blockEnd);

          loadAllFromNode(blockStart);
        }
      }
      else{
        strNode *blockStart = nodeRootEnd->down[1];
        strNode *blockEnd   = lastNode(blockStart);

        nodeRootEnd->down.erase(nodeRootEnd->down.begin() + 1,
                                nodeRootEnd->down.begin() + 2);

        // Load all down's before popping [{] and [}]'s
        const int blockDownCount = blockStart->down.size();

        for(int i = 0; i < blockDownCount; ++i)
          loadAllFromNode( blockStart->down[i] );

        loadBlocksFromLastNode(nodeRootEnd, 1);

        popAndGoRight(blockStart);
        popAndGoLeft(blockEnd);

        loadAllFromNode(blockStart);
      }

      return nextNode;
    }

    strNode* statement::loadWhileFromNode(const int st,
                                          strNode *nodeRoot,
                                          strNode *nodeRootEnd){
      strNode *nextNode = nodeRootEnd ? nodeRootEnd->right : NULL;

      if(nodeRoot)
        nodeRoot->left = NULL;
      if(nodeRootEnd)
        nodeRootEnd->right = NULL;

      const int downCount = nodeRootEnd->down.size();

      if(downCount == 1)
        return loadFromNode(nextNode);

      else{
        strNode *blockStart = nodeRoot->down[1];
        strNode *blockEnd   = lastNode(blockStart);

        nodeRoot->down.erase(nodeRoot->down.begin() + 1,
                             nodeRoot->down.begin() + 2);

        // Load all down's before popping [{] and [}]'s
        const int downCount = blockStart->down.size();

        for(int i = 0; i < downCount; ++i)
          loadAllFromNode( blockStart->down[i] );

        loadBlocksFromLastNode(nodeRootEnd, 1);

        popAndGoRight(blockStart);
        popAndGoLeft(blockEnd);

        loadAllFromNode(blockStart);
      }

      return nextNode;
    }

    strNode* statement::loadIfFromNode(const int st_,
                                       strNode *nodeRoot,
                                       strNode *nodeRootEnd){
      int st = st_;
      strNode *nextNode;

      do {
        nextNode = nodeRootEnd ? nodeRootEnd->right : NULL;

        if(nodeRoot)
          nodeRoot->left = NULL;
        if(nodeRootEnd)
          nodeRootEnd->right = NULL;

        statement *newStatement = new statement(depth + 1,
                                                st, this,
                                                nodeRoot, nodeRootEnd);

        addStatement(newStatement);

        const int downCount = nodeRootEnd->down.size();

        if(((downCount == 1) && (st != elseStatementType)) ||
           ((downCount == 0) && (st == elseStatementType))){
          // if()         or    else
          //   statement;         statement;

          nextNode = newStatement->loadFromNode(nextNode);

          if(st == elseStatementType)
            break;
        }
        else{
          int blockPos = (st != elseStatementType) ? 1 : 0;

          strNode *blockStart = nodeRoot->down[blockPos];
          strNode *blockEnd   = lastNode(blockStart);

          nodeRoot->down.erase(nodeRoot->down.begin() + blockPos,
                               nodeRoot->down.begin() + blockPos + 1);

          // Load all down's before popping [{] and [}]'s
          const int blockDownCount = blockStart->down.size();

          for(int i = 0; i < blockDownCount; ++i)
            newStatement->loadAllFromNode( blockStart->down[i] );

          loadBlocksFromLastNode(nodeRootEnd, blockPos);

          popAndGoRight(blockStart);
          popAndGoLeft(blockEnd);

          newStatement->loadAllFromNode(blockStart);

          break;
        }

        if(nextNode == NULL)
          break;

        nodeRoot = nodeRootEnd = nextNode;

        st = statementType(nodeRootEnd);

        if(st & invalidStatementType){
          std::cout << "Not a valid statement:\n";
          prettyString(nodeRoot, "", false);
          throw 1;
        }

      } while((st == elseIfStatementType) ||
              (st == elseStatementType));

      return nextNode;
    }

    // [-] Missing
    strNode* statement::loadSwitchFromNode(const int st,
                                           strNode *nodeRoot,
                                           strNode *nodeRootEnd){
      strNode *nextNode = nodeRootEnd ? nodeRootEnd->right : NULL;

      if(nodeRoot)
        nodeRoot->left = NULL;
      if(nodeRootEnd)
        nodeRootEnd->right = NULL;

      loadBlocksFromLastNode(nodeRootEnd);

      return nextNode;
    }

    strNode* statement::loadGotoFromNode(const int st,
                                         strNode *nodeRoot,
                                         strNode *nodeRootEnd){
      strNode *nextNode = nodeRootEnd ? nodeRootEnd->right : NULL;

      if(nodeRoot)
        nodeRoot->left = NULL;
      if(nodeRootEnd)
        nodeRootEnd->right = NULL;

      loadBlocksFromLastNode(nodeRootEnd);

      return nextNode;
    }

    strNode* statement::loadFunctionDefinitionFromNode(const int st,
                                                       strNode *nodeRoot,
                                                       strNode *nodeRootEnd){
      strNode *nextNode = nodeRootEnd ? nodeRootEnd->right : NULL;

      if(nodeRoot)
        nodeRoot->left = NULL;
      if(nodeRootEnd)
        nodeRootEnd->right = NULL;

      strNode *blockStart = nodeRootEnd->down[1];
      strNode *blockEnd   = lastNode(blockStart);

      nodeRootEnd->down.erase(nodeRootEnd->down.begin() + 1,
                              nodeRootEnd->down.begin() + 2);

      // Load all down's before popping [{] and [}]'s
      const int downCount = blockStart->down.size();

      for(int i = 0; i < downCount; ++i)
        loadAllFromNode( blockStart->down[i] );

      if(blockStart->right == blockEnd){
        popAndGoRight(blockStart);
        popAndGoLeft(blockEnd);

        return nextNode;
      }

      popAndGoRight(blockStart);
      popAndGoLeft(blockEnd);

      loadAllFromNode(blockStart);

      return nextNode;
    }

    strNode* statement::loadFunctionPrototypeFromNode(const int st,
                                                      strNode *nodeRoot,
                                                      strNode *nodeRootEnd){
      strNode *nextNode = nodeRootEnd ? nodeRootEnd->right : NULL;

      if(nodeRoot)
        nodeRoot->left = NULL;
      if(nodeRootEnd)
        nodeRootEnd->right = NULL;

      return nextNode;
    }

    strNode* statement::loadBlockFromNode(const int st,
                                          strNode *nodeRoot,
                                          strNode *nodeRootEnd){
      strNode *nextNode = nodeRootEnd ? nodeRootEnd->right : NULL;

      // Blocks don't have stuff, they just provide a new scope
      //   Hence, nodeStart = nodeEnd = NULL
      expCount = 0;
      exp      = NULL;

      nodeStart = nodeEnd = NULL;

      // Load all down's before popping [{] and [}]'s
      const int downCount = nodeRoot->down.size();

      for(int i = 0; i < downCount; ++i)
        loadAllFromNode( nodeRoot->down[i] );

      if(nodeRoot->right == nodeRootEnd){
        popAndGoRight(nodeRoot);
        popAndGoLeft(nodeRootEnd);

        return nextNode;
      }

      popAndGoRight(nodeRoot);
      popAndGoLeft(nodeRootEnd);

      loadAllFromNode(nodeRoot);

      return nextNode;
    }

    // [-] Missing
    strNode* statement::loadStructFromNode(const int st,
                                           strNode *nodeRoot,
                                           strNode *nodeRootEnd){
      strNode *nextNode = nodeRootEnd ? nodeRootEnd->right : NULL;

      if(nodeRoot)
        nodeRoot->left = NULL;
      if(nodeRootEnd)
        nodeRootEnd->right = NULL;


      typeDef &td = *(new typeDef);
      td.loadFromNode(*this, nodeRoot);

      // [--]
      std::cout << "typePtr = " << td << '\n';

      loadBlocksFromLastNode(nodeRootEnd);

      return nextNode;
    }

    // [-] Missing
    strNode* statement::loadBlankFromNode(const int st,
                                          strNode *nodeRoot,
                                          strNode *nodeRootEnd){
      strNode *nextNode = nodeRootEnd ? nodeRootEnd->right : NULL;

      if(nodeRoot)
        nodeRoot->left = NULL;
      if(nodeRootEnd)
        nodeRootEnd->right = NULL;

      loadBlocksFromLastNode(nodeRootEnd);

      return nextNode;
    }

    // [-] Missing
    strNode* statement::loadMacroFromNode(const int st,
                                          strNode *nodeRoot,
                                          strNode *nodeRootEnd){
      strNode *nextNode = nodeRootEnd ? nodeRootEnd->right : NULL;

      if(nodeRoot)
        nodeRoot->left = NULL;
      if(nodeRootEnd)
        nodeRootEnd->right = NULL;

      loadBlocksFromLastNode(nodeRootEnd);

      return nextNode;
    }

    varInfo* statement::addVariable(const varInfo &info,
                                    statement *origin){
      scopeVarMapIterator it = scopeVarMap.find(info.name);

      if(it != scopeVarMap.end()       &&
         !info.hasDescriptor("extern") &&
         !((info.typeInfo & functionType) && ((it->second)->typeInfo & protoType))){

        std::cout << "Variable [" << info.name << "] defined in:\n"
                  << *origin
                  << "is already defined in:\n"
                  << *this;
        throw 1;
      }

      varInfo *&newInfo = scopeVarMap[info.name];

      newInfo = new varInfo(info);

      if(origin == NULL)
        varOriginMap[newInfo] = this;
      else{
        varOriginMap[newInfo]          = origin;
        origin->scopeVarMap[info.name] = newInfo;
      }

      return newInfo;
    }

    void statement::addStatement(statement *newStatement){
      if(statementStart != NULL){
        ++statementCount;
        statementEnd = statementEnd->push(newStatement);
      }
      else{
        statementCount = 1;
        statementStart = new node<statement*>(newStatement);
        statementEnd   = statementStart;
      }
    }

    statement* statement::clone() const {
      statement *newStatement = new statement(depth,
                                              type, up,
                                              NULL, NULL);

      if(expCount){
        newStatement->exp = new expNode*[expCount];

        for(int i = 0; i < expCount; ++i)
          newStatement->exp[i] = exp[i]->clone(*newStatement);
      }

      if(nodeStart){
        newStatement->nodeStart = nodeStart->clone();
        newStatement->nodeEnd   = lastNode(newStatement->nodeStart);
      }

      newStatement->scopeVarMap = scopeVarMap;

      newStatement->statementCount = statementCount;

      newStatement->statementStart = NULL;
      newStatement->statementEnd   = NULL;

      if(statementCount == 0)
        return newStatement;

      statementNode *nodePos = statementStart;

      for(int i = 0; i < statementCount; ++i){
        newStatement->addStatement( nodePos->value->clone() );
        nodePos = nodePos->right;
      }

      return newStatement;
    }

    void statement::printVariablesInStatement(){
      scopeVarMapIterator it = scopeVarMap.begin();

      while(it != scopeVarMap.end()){
        std::cout << "  " << *(it->second) << '\n';

        ++it;
      }
    }

    void statement::printVariablesInScope(){
      if(up)
        up->printVariablesInScope();

      printVariablesInStatement();
    }

    void statement::printTypesInScope(){
      if(up)
        up->printTypesInScope();

      printTypesInStatement();
    }

    void statement::printTypesInStatement(){
      scopeTypeMapIterator it = scopeTypeMap.begin();

      while(it != scopeTypeMap.end()){
        std::cout << (it->first) << '\n';

        ++it;
      }
    }

    void statement::printTypeDefsInStatement(){
      scopeTypeMapIterator it = scopeTypeMap.begin();

      while(it != scopeTypeMap.end()){
        std::cout << (it->second)->print("  ") << '\n';

        ++it;
      }
    }

    // autoMode: Handles newlines and tabs
    std::string statement::prettyString(strNode *nodeRoot,
                                        const std::string &tab_,
                                        const bool autoMode) const {
      strNode *nodePos = nodeRoot;

      std::string tab = tab_;
      std::string ret = "";

      while(nodePos){
        if(nodePos->type & operatorType){

          if(nodePos->type & binaryOperatorType){

            // char *blah
            if(nodeHasQualifier(nodePos)){

              // [char ][*][blah]
              // or
              // [int ][a][ = ][0][, ][*][b][ = ][1][;]
              //                       ^
              if(nodePos->left &&
                 ((nodePos->left->type & descriptorType) ||
                  (nodePos->left->value == ","))){
                ret += *nodePos;

                // [const ][*][ const]
                if(nodePos->right &&
                   (nodePos->right->type & descriptorType) &&
                   !(nodePos->right->value == "*"))
                  ret += ' ';
              }
              else{
                ret += " ";
                ret += *nodePos;
                ret += " ";
              }
            }
            // [+] and [-]
            else if(nodePos->type & unitaryOperatorType){
              // (-blah ... )
              if(nodePos->left &&
                 !(nodePos->left->type & (presetValue | unknownVariable)) )
                ret += *nodePos;
              // a - b
              else{
                ret += " ";
                ret += *nodePos;
                ret += " ";
              }
            }
            else if(nodePos->value == ","){
              ret += ", ";
            }
            else if((nodePos->value == ".") || (nodePos->value == "::")){
              if(((nodePos->left == NULL) ||
                  !(nodePos->left->type & unknownVariable)) ||
                 ((nodePos->right == NULL) ||
                  !(nodePos->right->type & unknownVariable))){
                if(nodePos->left){
                  nodePos->up->print();
                  std::cout << "1. Error on:\n";
                  nodePos->left->print("  ");
                }
                else{
                  std::cout << "2. Error on:\n";
                  nodePos->print("  ");
                }

                throw 1;
              }

              ret += *nodePos;
            }
            else{
              ret += " ";
              ret += *nodePos;
              ret += " ";
            }

          }
          // [++] and [--]
          else if(nodePos->type & unitaryOperatorType){
            ret += *nodePos;
          }
          else if(nodePos->type & ternaryOperatorType){
            ret += " ? ";

            nodePos = nodePos->right;

            if((nodePos->right) == NULL){
              std::cout << "3. Error on:\n";
              nodePos->left->print("  ");
              throw 1;
            }

            if((nodePos->down).size())
              ret += prettyString(nodePos, "", autoMode);
            else
              ret += *nodePos;

            ret += " : ";

            if(((nodePos->right) == NULL)       ||
               ((nodePos->right->right) == NULL)){
              std::cout << "4. Error on:\n";
              nodePos->left->left->print("  ");
              throw 1;
            }

            nodePos = nodePos->right->right;

            if((nodePos->down).size())
              ret += prettyString(nodePos, "", autoMode);
            else
              ret += *nodePos;
          }
        }

        else if(nodePos->type & brace){
          if(nodePos->type & startSection){
            // a[] = {};
            if(nodePos->up->type & binaryOperatorType){
              ret += "{ ";
            }
            else{
              // Case: function(...) const {
              if( (((nodePos->sideDepth) != 0) &&
                   ((nodePos->up->down[nodePos->sideDepth - 1]->type & parentheses) ||
                    (nodePos->up->down[nodePos->sideDepth - 1]->value == "const")) )

                  || (nodePos->up->type & (occaKeywordType | flowControlType)))
                ret += " {\n" + tab + "  ";
              else
                ret += tab + "{\n";
            }

            tab += "  ";
          }
          else{
            tab = tab.substr(0, tab.size() - 2);

            // a[] = {};
            if(nodePos->up &&
               (nodePos->up->type & binaryOperatorType))
              ret += " }";
            else{
              ret += '}';

              //   }
              // }
              if((nodePos->up == NULL) ||
                 ((nodePos->up->right) &&
                  (nodePos->up->right->type == endBrace)))
                ret += "\n" + tab.substr(0, tab.size() - 2);
              else
                ret += "\n" + tab;
            }
          }
        }

        else if(nodePos->type == endParentheses){
          ret += ")";

          // if(...) statement
          if(autoMode)
            if((nodePos->up->type & flowControlType) &&
               (((nodePos->sideDepth) >= (nodePos->up->down.size() - 1)) ||
                !(nodePos->up->down[nodePos->sideDepth + 1]->type & brace))){

              ret += "\n" + tab + "  ";
            }
        }

        else if(nodePos->type & endStatement){
          ret += *nodePos;

          // for(){
          //   ...;
          // }
          if((nodePos->right == NULL) ||
             ((nodePos->right) &&
              (nodePos->right->type & brace))){

            ret += "\n" + tab.substr(0, tab.size() - 2);
          }
          //   blah;
          // }
          else if(!(nodePos->up)                    ||
                  !(nodePos->up->type & flowControlType) ||
                  !(nodePos->up->value == "for")){

            ret += "\n" + tab;
          }
          // Don't add newlines to for(A;B;C)
          else
            ret += " ";
        }

        else if(nodeHasDescriptor(nodePos)){
          ret += *nodePos;

          if(nodePos->right &&
             // [static ][const ][float ][variable]
             ((nodePos->right->type & (presetValue    |
                                       unknownVariable)) ||
              nodeHasDescriptor(nodePos->right))){

            ret += " ";
          }
        }

        else if(nodePos->type & flowControlType){
          ret += *nodePos;

          if(autoMode)
            if(nodePos->down.size() == 0)
              ret += '\n' + tab + "  ";
        }

        else if(nodePos->type & specialKeywordType){
          if(nodePos->value == "case")
            ret += "case";
          else if(nodePos->value == "default")
            ret += "default";
          else if(nodePos->value == "break")
            ret += "break";
          else if(nodePos->value == "continue")
            ret += "continue";
          else if(nodePos->value == "return"){
            ret += "return";

            if(nodePos->right || nodePos->down.size())
              ret += ' ';
          }
          else if(nodePos->value == "goto")
            ret += "goto ";
          else
            ret += *nodePos;
        }
        else if(nodePos->type & macroKeywordType){
          ret += *nodePos;

          ret += '\n' + tab;
        }
        else
          ret += *nodePos;

        const int downCount = (nodePos->down).size();

        for(int i = 0; i < downCount; ++i){
          strNode *downNode = nodePos->down[i];

          ret += prettyString(downNode, tab, autoMode);
        }

        nodePos = nodePos->right;
      }

      return ret;
    }

    statement::operator std::string() const {
      const std::string tab = getTab();

      statementNode *statementPos = statementStart;

      // OCCA For's
      if(type == (occaStatementType | forStatementType)){
        std::string ret = tab + nodeStart->value + " {\n";

        while(statementPos){
          ret += (std::string) *(statementPos->value);
          statementPos = statementPos->right;
        }

        ret += tab + "}\n";

        return ret;
      }

      else if(type & (simpleStatementType | gotoStatementType)){
        return tab + prettyString(nodeStart, "", false);
      }

      else if(type & flowStatementType){
        std::string ret = tab + prettyString(nodeStart, "", false);

        if(statementCount > 1)
          ret += " {";

        ret += '\n';

        while(statementPos){
          ret += (std::string) *(statementPos->value);
          statementPos = statementPos->right;
        }

        if(statementCount > 1)
          ret += tab + "}\n";

        return ret;
      }

      else if(type & functionStatementType){
        if(type & functionDefinitionType){
          std::string ret = prettyString(nodeStart, "", false);

          ret += " {\n";

          while(statementPos){
            ret += (std::string) *(statementPos->value);
            statementPos = statementPos->right;
          }

          ret += tab + "}\n";

          return ret;
        }
        else if(type & functionPrototypeType)
          return tab + prettyString(nodeStart, "", false);
      }
      else if(type & blockStatementType){
        std::string ret = "";

        if(0 <= depth)
          ret += tab + "{\n";

        while(statementPos){
          ret += (std::string) *(statementPos->value);
          statementPos = statementPos->right;
        }

        if(0 <= depth)
          ret += tab + "}\n";

        return ret;
      }
      else if(type & structStatementType){
        return tab + prettyString(nodeStart, "", false);
      }
      else if(type & macroStatementType){
        return tab + prettyString(nodeStart, "", false);
      }

      return tab + prettyString(nodeStart, "", false);
    }

    std::ostream& operator << (std::ostream &out, const statement &s){
      out << (std::string) s;

      return out;
    }
    //==============================================


    //---[ Exp Node ]-------------------------------
    expNode::expNode(statement &s) :
      sInfo(s),

      value(""),
      info(expType::root),

      up(NULL),

      leafCount(0),
      leaves(NULL),
      var(NULL),
      type(NULL) {}

    expNode::expNode(expNode &up_) :
      sInfo(up_.sInfo),

      value(""),
      info(expType::root),

      up(&up_),

      leafCount(0),
      leaves(NULL),
      var(NULL),
      type(NULL) {}

    void expNode::loadFromNode(strNode *n){
      strNode *nClone = n->clone();
      initLoadFromNode(nClone);

      initOrganization();
      organizeLeaves();

      occa::parserNamespace::free(nClone);
    }

    void expNode::initLoadFromNode(strNode *n){
      strNode *nodeRoot = n;
      strNode *nodePos  = nodeRoot;

      while(nodePos){
        leafCount += (1 + nodePos->down.size());
        nodePos = nodePos->right;
      }

      if(leafCount == 0)
        return;

      nodePos = nodeRoot;

      leaves = new expNode*[leafCount];
      leafCount = 0;

      while(nodePos){
        expNode *&leaf = leaves[leafCount++];

        leaf        = new expNode(*this);
        leaf->up    = this;
        leaf->value = nodePos->value;

        if(nodePos->type & unknownVariable){

          // [-] Temp until I merge with parser
          if(nodePos->down.size() &&
             (nodePos->down[0]->type & parentheses)){

            leaf->info = expType::function;
          }
          else{
            leaf->info = expType::variable;
          }
        }

        else if(nodePos->type & presetValue){
          leaf->info = expType::presetValue;
        }

        else if(nodePos->type & descriptorType){
          // Case const int [*] const [*] a;
          if(nodePos->type & operatorType){
            // [--] Check for custom variable-type when
            //        putting this in parser

            if(nodePos->value == "*"){
              bool starIsQualifier = true;
              int starCount = 1;

              strNode *nodePos2 = nodePos->right;

              while(nodePos2 &&
                    (nodePos2->value == "*")){
                ++starCount;
                nodePos2 = nodePos2->right;
              }

              if(nodePos2){
                varInfo *vInfo = sInfo.hasVariableInScope(nodePos2->value);

                if(vInfo){

                }
              }

              if(starIsQualifier){
                leaf->info = expType::qualifier;

                for(int i = 1; i < starCount; ++i){
                  expNode *&leaf2 = leaves[leafCount++];

                  leaf2        = new expNode(*this);
                  leaf2->up    = this;
                  leaf2->value = nodePos->value;
                  leaf2->info  = expType::qualifier;

                  nodePos = nodePos->right;
                }
              }
            }
          }
          else{
            if(nodePos->type & qualifierType)
              leaf->info = expType::qualifier;
            else
              leaf->info = expType::type;
          }
        }

        const int downCount = nodePos->down.size();

        for(int i = 0; i < downCount; ++i){
          strNode *downNode = nodePos->down[i];
          strNode *lastDown = lastNode(downNode);

          std::string sValue = downNode->value;

          // Get rid of ()'s and stuff
          popAndGoRight(downNode);
          popAndGoLeft(lastDown);

          expNode *&sLeaf = leaves[leafCount++];

          sLeaf        = new expNode(*this);
          sLeaf->value = sValue;
          sLeaf->info  = expType::C;

          // Case: ()
          if(lastDown != NULL)
            sLeaf->initLoadFromNode(downNode);
        }

        nodePos = nodePos->right;
      }
    }

    void expNode::initOrganization(){
      // Init ()'s bottom -> up
      for(int i = 0; i < leafCount; ++i){
        if(leaves[i]->leafCount)
          leaves[i]->initOrganization();
      }

      //---[ Level 0 ]------
      // [a][::][b]
      mergeNamespaces();

      // [const] int [*] x
      mergeQualifiers();

      // [[const] [int] [*]] x
      mergeTypes();

      // [[[const] [int] [*]] [x]]
      mergeVariables();

      // [qualifiers] [type] (*[name]) ([args])
      mergeFunctionPointers();
      //====================
    }

    void expNode::organizeLeaves(){
      // Organize leaves bottom -> up
      for(int i = 0; i < leafCount; ++i){
        if((leaves[i]->leafCount) &&
           !(leaves[i]->info & (expType::type      |
                                expType::qualifier |
                                expType::function  |
                                expType::functionPointer))){

          leaves[i]->organizeLeaves();
        }
      }

      //---[ Level 1 ]------
      // class(...), class{1,2,3}
      mergeClassConstructs();

      // static_cast<>()
      mergeCasts();

      // func()
      mergeFunctionCalls();

      organizeLeaves(1);
      //====================

      //---[ Level 2 ]------
      // (class) x
      mergeClassCasts();

      // sizeof x
      mergeSizeOf();

      // new, new [], delete, delete []
      mergeNewsAndDeletes();

      organizeLeaves(2);
      //====================

      //---[ Level 3-14 ]---
      for(int i = 3; i <= 14; ++i)
        organizeLeaves(i);
      //====================

      //---[ Level 15 ]-----
      // throw x
      mergeThrows();
      //====================

      //---[ Level 16 ]-----
      organizeLeaves(16);
      //====================
    }

    void expNode::organizeLeaves(const int level){
      int leafPos = 0;

      while(leafPos < leafCount){
        if(leaves[leafPos]->leafCount){
          ++leafPos;
          continue;
        }

        opLevelMapIterator it = opLevelMap[level].find(leaves[leafPos]->value);

        if(it == opLevelMap[level].end()){
          ++leafPos;
          continue;
        }

        const int levelType = it->second;

        if(levelType & unitaryOperatorType){
          bool updateNow = true;

          // Cases:  1 + [-]1
          //         (+1)
          if((leaves[leafPos]->value.size() == 1) &&
             ((leaves[leafPos]->value[0] == '+') ||
              (leaves[leafPos]->value[0] == '-'))){

            if(leafPos &&
               !(leaves[leafPos - 1]->info & expType::LCR))
              updateNow = false;
          }

          if(updateNow){
            if(levelType & lUnitaryOperatorType)
              leafPos = mergeLeftUnary(leafPos);
            else
              leafPos = mergeRightUnary(leafPos);
          }
          else
            ++leafPos;
        }
        else if(levelType & binaryOperatorType)
          leafPos = mergeBinary(leafPos);
        else if(levelType & ternaryOperatorType)
          leafPos = mergeTernary(leafPos);
        else
          ++leafPos;
      }
    }

    int expNode::mergeRange(const int newLeafType,
                            const int leafPosStart,
                            const int leafPosEnd){
      expNode *newLeaf = new expNode(*this);

      newLeaf->up        = this;
      newLeaf->info      = newLeafType;
      newLeaf->leafCount = (leafPosEnd - leafPosStart + 1);
      newLeaf->leaves    = new expNode*[newLeaf->leafCount];

      for(int i = 0; i < newLeaf->leafCount; ++i){
        newLeaf->leaves[i]     = leaves[leafPosStart + i];
        newLeaf->leaves[i]->up = newLeaf;
      }

      leaves[leafPosStart] = newLeaf;

      for(int i = (leafPosEnd + 1); i < leafCount; ++i)
        leaves[leafPosStart + i - leafPosEnd] = leaves[i];

      leafCount -= (newLeaf->leafCount - 1);

      return (leafPosStart + 1);
    }

    // [a][::][b]
    void expNode::mergeNamespaces(){
    }

    int expNode::mergeNamespace(const int leafPos){
      return 0;
    }

    // [const] int x
    void expNode::mergeQualifiers(){
      int leafPos = 0;

      while(leafPos < leafCount){
        if(leafPos < leafCount)
          std::cout << "1. leaves[leafPos] = " << leaves[leafPos]->value << '\n';

        if(leaves[leafPos]->info & expType::qualifier){
          int leafPosStart = leafPos;
          int leafPosEnd   = leafPos;

          while((leafPosEnd < leafCount) &&
                (leaves[leafPosEnd]->info & expType::qualifier))
            ++leafPosEnd;

          for(int i = leafPosStart; i <= leafPosEnd; ++i)
            std::cout << "leaves[i]->value = " << leaves[i]->value << '\n';

          leafPos = mergeRange(expType::qualifier,
                               leafPosStart, leafPosEnd - 1);

          print();
        }
        else
          ++leafPos;

        if(leafPos < leafCount)
          std::cout << "2. leaves[leafPos] = " << leaves[leafPos]->value << '\n';
      }
    }

    // [[const] [int] [*]] x
    void expNode::mergeTypes(){
      int leafPos = 0;

      while(leafPos < leafCount){
        if(leaves[leafPos]->info & expType::type){
          int leafPosStart = leafPos;
          int leafPosEnd   = leafPos;

          if(leafPos &&
             (leaves[leafPos - 1]->info & expType::qualifier)){

            --leafPosStart;
          }

          if(((leafPos + 1) < leafCount) &&
             (leaves[leafPos + 1]->info & expType::qualifier)){

            ++leafPosEnd;
          }

          leafPos = mergeRange(expType::type,
                               leafPosStart, leafPosEnd);
        }
        else
          ++leafPos;
      }
    }

    // [[[const] [int] [*]] [x]]
    void expNode::mergeVariables(){
      int leafPos = 0;

      while(leafPos < leafCount){
        if((leaves[leafPos]->info & expType::type) && // [[const] [int] [*]]
           ((leafPos + 1) < leafCount)             && // [x]
           (leaves[leafPos + 1]->info & expType::variable)){

          leafPos = mergeRange(expType::variable,
                               leafPos, leafPos + 1);
        }
        else
          ++leafPos;
      }
    }

    // 1 [type]                           2 [(]       3 [(]
    // [[qualifiers] [type] [qualifiers]] [(*[name])] [([args])]
    void expNode::mergeFunctionPointers(){
      int leafPos = 0;

      while(leafPos < leafCount){
        if((leaves[leafPos]->info & expType::type)   &&     // 1
           ((leafPos + 2) < leafCount)               &&     // Has 2 & 3
           (leaves[leafPos + 1]->info == expType::C) &&     // 2
           (leaves[leafPos + 2]->info == expType::C) &&     // 3
           (leaves[leafPos + 1]->leaves[0]->value == "*")){ // 2.5

          expNode *newLeaf = new expNode(*this);

          newLeaf->up        = this;
          newLeaf->info      = expType::functionPointer;
          newLeaf->leafCount = 3;
          newLeaf->leaves    = new expNode*[3];
          newLeaf->leaves[0] = leaves[leafPos];
          newLeaf->leaves[1] = leaves[leafPos + 1]->leaves[1];
          newLeaf->leaves[2] = leaves[leafPos + 2];

          // Merge arguments in [leafPos + 2]
          newLeaf->leaves[2]->mergeArguments();

          // Don't kill the name of the function pointer
          leaves[leafPos + 1]->leafCount = 1;
          freeLeaf(leafPos + 1);

          leaves[leafPos] = newLeaf;

          for(int i = (leafPos + 3); i < leafCount; ++i)
            leaves[i - 2] = leaves[i];

          ++leafPos;
          leafCount -= 2;
        }
        else
          ++leafPos;
      }
    }

    // class(...), class{1,2,3}
    void expNode::mergeClassConstructs(){
    }

    // static_cast<>()
    void expNode::mergeCasts(){
    }

    // func()
    void expNode::mergeFunctionCalls(){
    }

    void expNode::mergeArguments(){
      for(int i = 0; i < leafCount; i += 2){
        leaves[i/2] = leaves[i];

        if((i + 1) < leafCount)
          freeLeaf(i + 1);
      }

      leafCount = ((leafCount / 2) + 1);
    }

    // (class) x
    void expNode::mergeClassCasts(){
    }

    // sizeof x
    void expNode::mergeSizeOf(){
    }

    // new, new [], delete, delete []
    void expNode::mergeNewsAndDeletes(){
    }

    // throw x
    void expNode::mergeThrows(){
    }

    // [++]i
    int expNode::mergeLeftUnary(const int leafPos){
      expNode *leaf  = leaves[leafPos];
      expNode *sLeaf = leaves[leafPos + 1];

      for(int i = (leafPos + 1); i < leafCount; ++i)
        leaves[i - 1] = leaves[i];

      --leafCount;

      leaf->info      = expType::L;
      leaf->leafCount = 1;
      leaf->leaves    = new expNode*[1];
      leaf->leaves[0] = sLeaf;

      sLeaf->up = leaf;

      return (leafPos + 1);
    }

    // i[++]
    int expNode::mergeRightUnary(const int leafPos){
      expNode *leaf  = leaves[leafPos];
      expNode *sLeaf = leaves[leafPos - 1];

      leaves[leafPos - 1] = leaf;

      --leafCount;

      for(int i = leafPos; i < leafCount; ++i)
        leaves[i] = leaves[i + 1];

      leaf->info      = expType::R;
      leaf->leafCount = 1;
      leaf->leaves    = new expNode*[1];
      leaf->leaves[0] = sLeaf;

      sLeaf->up = leaf;

      return (leafPos + 1);
    }

    // a [+] b
    int expNode::mergeBinary(const int leafPos){
      expNode *leaf   = leaves[leafPos];
      expNode *sLeafL = leaves[leafPos - 1];
      expNode *sLeafR = leaves[leafPos + 1];

      leaves[leafPos - 1] = leaf;

      leafCount -= 2;

      for(int i = leafPos; i < leafCount; ++i)
        leaves[i] = leaves[i + 2];

      leaf->info      = (expType::L | expType::R);
      leaf->leafCount = 2;
      leaf->leaves    = new expNode*[2];
      leaf->leaves[0] = sLeafL;
      leaf->leaves[1] = sLeafR;

      sLeafL->up = leaf;
      sLeafR->up = leaf;

      return leafPos;
    }

    // a [?] b : c
    int expNode::mergeTernary(const int leafPos){
      expNode *leaf   = leaves[leafPos];
      expNode *sLeafL = leaves[leafPos - 1];
      expNode *sLeafC = leaves[leafPos + 1];
      expNode *sLeafR = leaves[leafPos + 3];

      leaves[leafPos - 1] = leaf;

      leafCount -= 4;

      for(int i = leafPos; i < leafCount; ++i)
        leaves[i] = leaves[i + 4];

      leaf->info      = (expType::L | expType::C | expType::R);
      leaf->leafCount = 3;
      leaf->leaves    = new expNode*[3];
      leaf->leaves[0] = sLeafL;
      leaf->leaves[1] = sLeafC;
      leaf->leaves[2] = sLeafR;

      sLeafL->up = leaf;
      sLeafC->up = leaf;
      sLeafR->up = leaf;

      return leafPos;
    }

    //---[ Custom Type Info ]---------
    bool expNode::qualifierEndsWithStar() const {
      if( !(info & expType::qualifier) )
        return false;

      if(leafCount)
        return leaves[leafCount - 1]->qualifierEndsWithStar();
      else
        return (value == "*");
    }

    bool expNode::typeEndsWithStar() const {
      if( !(info & expType::type) ||
          (leafCount == 0) )
        return false;

      if(leaves[leafCount - 1]->info & expType::qualifier)
        return leaves[leafCount - 1]->qualifierEndsWithStar();

      return false;
    }
    //================================

    expNode* expNode::clone(statement &s){
      expNode &newRoot = *(new expNode(s));

      newRoot.value = value;

      newRoot.leafCount = leafCount;
      newRoot.var       = var;
      newRoot.type      = type;

      if(leafCount){
        newRoot.leaves = new expNode*[leafCount];

        for(int i = 0; i < leafCount; ++i)
          newRoot.leaves[i] = clone(leaves[i]);
      }

      return &newRoot;
    }

    expNode* expNode::clone(expNode *original){
      expNode &newLeaf = *(new expNode(*this));
      expNode &o = *original;

      newLeaf.value = o.value;

      newLeaf.leafCount = o.leafCount;
      newLeaf.var       = o.var;
      newLeaf.type      = o.type;

      if(o.leafCount){
        newLeaf.leaves = new expNode*[o.leafCount];

        for(int i = 0; i < o.leafCount; ++i)
          newLeaf.leaves[i] = o.clone(o.leaves[i]);
      }

      return &newLeaf;
    }

    void expNode::freeLeaf(const int leafPos){
      leaves[leafPos]->free();
      delete leaves[leafPos];
    }

    void expNode::free(){
      for(int i = 0; i < leafCount; ++i){
        leaves[i]->free();
        delete leaves[i];
      }

      delete [] leaves;
    }

    void expNode::print(const std::string &tab){
      std::cout << tab << "[";

      bool printedSomething = false;
      for(int i = 0; i < 10; ++i){
        if(info & (1 << i)){
          if(printedSomething)
            std::cout << ',';

          std::cout << i;

          printedSomething = true;
        }
      }

      std::cout << "] " << value << '\n';

      for(int i = 0; i < leafCount; ++i)
        leaves[i]->print(tab + "    ");
    }

    expNode::operator std::string () const {
      std::stringstream ss;
      ss << *this;
      return ss.str();
    }

    std::ostream& operator << (std::ostream &out, const expNode &n){
      switch(n.info){
      case (expType::root):{
        for(int i = 0; i < n.leafCount; ++i)
          out << *(n.leaves[i]);

        break;
      }

      case (expType::L):{
        out << n.value << *(n.leaves[0]);

        break;
      }
      case (expType::R):{
        out << *(n.leaves[0]) << n.value;

        break;
      }
      case (expType::L | expType::R):{
        out << *(n.leaves[0]) << n.value << *(n.leaves[1]);

        break;
      }
      case (expType::L | expType::C | expType::R):{
        out << *(n.leaves[0]) << '?' << *(n.leaves[1]) << ':' << *(n.leaves[2]);

        break;
      }
      case expType::C:{
        const char startChar = n.value[0];

        out << startChar;

        for(int i = 0; i < n.leafCount; ++i)
          out << *(n.leaves[i]);

        out << (char) ((')' * (startChar == '(')) +
                       (']' * (startChar == '[')) +
                       ('}' * (startChar == '{')));

        break;
      }

      case expType::qualifier:{
        if(n.leafCount){
          int leafPos = 0;

          std::string strLeaf = (std::string) *(n.leaves[0]);
          bool lastStar = (strLeaf == "*");

          out << strLeaf;

          for(int i = 1; i < n.leafCount; ++i){
            std::string strLeaf = (std::string) *(n.leaves[i]);
            const bool thisStar = (strLeaf == "*");

            if( !(thisStar && lastStar) )
              out << ' ';

            out << strLeaf;

            lastStar = thisStar;
          }
        }
        else{
          out << n.value;
        }

        break;
      }
      case expType::type:{
        // [const] [int] [*]
        if(n.leafCount){
          for(int i = 0; i < (n.leafCount - 1); ++i)
            out << *(n.leaves[i]) << ' ';

          out << *(n.leaves[n.leafCount - 1]);
        }
        // [int]
        else{
          out << n.value;
        }

        break;
      }
      case expType::presetValue:{
        out << n.value;

        break;
      }
      case expType::variable:{
        // [[[const] [int] [*]] [x]]
        if(n.leafCount){
          out << *(n.leaves[0]);

          if( !(n.leaves[0]->typeEndsWithStar()) )
            out << ' ';

          out << *(n.leaves[1]);
        }
        // [x]
        else{
          out << n.value;
        }

        break;
      }
      case expType::function:{
        out << n.value << '(';

        if(n.leafCount){
          for(int i = 0; i < (n.leafCount - 1); ++i)
            out << *(n.leaves[i]) << ", ";

          out << *(n.leaves[n.leafCount - 1]);
        }

        out << ')';

        break;
      }
      case expType::functionPointer:{
        out << *(n.leaves[0]) << " (*" << *(n.leaves[1]) << ")"
            << '(';

        expNode *argNode = n.leaves[2];

        if(argNode->leafCount){
          for(int i = 0; i < (argNode->leafCount - 1); ++i)
            out << *(argNode->leaves[i]) << ", ";

          out << *(argNode->leaves[argNode->leafCount - 1]);
        }

        out << ')';

        break;
      }
      };

      return out;
    }
    //==============================================
  };
};
