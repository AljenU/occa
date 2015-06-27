#include "occaParserAnalyzer.hpp"
#include "occaParser.hpp"

namespace occa {
  namespace parserNS {
    //---[ Variable Dependencies ]----------------
    varDepInfo::varDepInfo() :
      info(depType::none),
      var(NULL),
      myNode(NULL),
      subNode(NULL) {}

    void varDepInfo::setup(int info_,
                           varInfo &var_,
                           smntDepInfo &sdInfo_,
                           varDepInfoNode &myNode_){
      info = info_;
      var  = &var_;

      sdInfo = &sdInfo_;

      myNode        = &myNode_;
      myNode->value = this;
    }

    int varDepInfo::startInfo(){
      if(subNode == NULL)
        return info;

      return subNode->value->info;
    }

    int varDepInfo::endInfo(){
      if(subNode == NULL)
        return info;

      return lastNode(subNode)->value->info;
    }
    //============================================


    //---[ Statement Dependencies ]---------------
    smntDepInfo::smntDepInfo() :
      s(NULL),
      myNode(NULL) {}

    void smntDepInfo::setup(statement &s_, smntDepInfoNode &myNode_){
      s = &s_;

      myNode        = &myNode_;
      myNode->value = this;

      expNode &flatRoot = *(s_.expRoot.makeFlatHandle());

      for(int i = 0; i < flatRoot.leafCount; ++i){
        expNode &leaf = flatRoot[i];

        if((leaf.info & expType::varInfo) == 0)
          continue;

        int leafDepInfo = getDepTypeFrom(leaf);

        if(leafDepInfo == depType::none)
          continue;

        varInfo &var = leaf.getVarInfo();

        varDepInfoNode *&vdNode = v2dMap[&var];

        varDepInfo &newVdInfo = *(new varDepInfo);
        newVdInfo.setup(leafDepInfo, var, *this, *(new varDepInfoNode));

        if(vdNode == NULL){
          vdNode = newVdInfo.myNode;

          setupNestedVdInfos(s_, var, vdNode);
        }
        else {
          varDepInfo &vdInfo = *(vdNode->value);

          // Copy vdInfo to the first subNode
          if(vdInfo.subNode == NULL){
            varDepInfo &vdInfo2 = *(new varDepInfo);
            vdInfo2.setup(vdInfo.info, var, *this, *(new varDepInfoNode));

            vdInfo.info    = depType::none;
            vdInfo.subNode = vdInfo2.myNode;
          }

          varDepInfoNode *endNode = lastNode(vdInfo.subNode);

          endNode->push(newVdInfo.myNode);
        }
      }

      expNode::freeFlatHandle(flatRoot);
    }

    void smntDepInfo::setupNestedVdInfos(statement &s_,
                                         varInfo &var,
                                         varDepInfoNode *vdNode){

      statement &sOrigin = *(s_.parser.varOriginMap[&var]);

      smntDepInfoNode *sdNode = myNode;

      while((sdNode           != NULL) &&
            (sdNode->value->s != &sOrigin)){

        smntDepInfo &sdInfo = *(sdNode->value);
        varDepInfo *vdInfo2 = sdInfo.has(var);

        if(vdInfo2 != NULL){
          varDepInfoNode &vdNode2 = *(vdInfo2->myNode);

          if(vdNode2.down == NULL)
            vdNode2.pushDown(vdNode);
          else
            lastNode(vdNode2.down)->push(vdNode);

          break;
        }
        else {
          varDepInfoNode *&vdNodeUp = sdInfo.v2dMap[&var];

          varDepInfo &vdInfoUp = *(new varDepInfo);
          vdInfoUp.setup(depType::none, var, sdInfo, *(new varDepInfoNode));

          vdNodeUp = vdInfoUp.myNode;

          vdNodeUp->pushDown(vdNode);

          vdNode = vdNodeUp;
        }

        sdNode = sdNode->up;
      }
    }

    int smntDepInfo::getDepTypeFrom(expNode &e){
      if((e.up == NULL) ||
         ((e.up->info & expType::operator_) == 0)){

        return depType::none;
      }

      expNode &eUp = *(e.up);

      if(eUp.value == "=")
        return depType::set;

      if(isAnUpdateOperator(eUp.value))
        return depType::update;

      return depType::none;
    }

    varDepInfo* smntDepInfo::has(varInfo &var){
      varToDepMapIterator it = v2dMap.find(&var);

      if(it == v2dMap.end())
        return NULL;

      return (it->second->value);
    }

    varDepInfo& smntDepInfo::operator () (varInfo &var){
      return *(has(var));
    }

    int smntDepInfo::startInfo(varInfo &var){
      varDepInfo *vdInfo = has(var);

      if(vdInfo == NULL)
        return depType::none;

      return vdInfo->startInfo();
    }

    int smntDepInfo::endInfo(varInfo &var){
      varDepInfo *vdInfo = has(var);

      if(vdInfo == NULL)
        return depType::none;

      return vdInfo->endInfo();
    }
    //============================================


    //---[ Dependency Map ]-----------------------
    depMap_t::depMap_t(statement &s) :
      parser(s.parser) {
      setup(s);
    }

    void depMap_t::setup(statement &s){
      setup(s, *(new smntDepInfo));
    }

    void depMap_t::setup(statement &s, smntDepInfo &sdInfo){
      smntDepInfoNode &sdNode = *(new smntDepInfoNode);
      sdNode.value = &sdInfo;

      sdInfo.setup(s, sdNode);

      s2vdMap[&s] = &sdInfo;

      statementNode *sn = s.statementStart;

      while(sn){
        statement &s2 = *(sn->value);

        setup(s2, *(new smntDepInfo));

        sn = sn->right;
      }
    }

    varDepInfo* depMap_t::has(varInfo &var){
      statement   &s      = *(parser.varOriginMap[&var]);
      smntDepInfo *sdInfo = has(s);

      if(sdInfo == NULL)
        return NULL;

      return sdInfo->has(var);
    }

    varDepInfo& depMap_t::operator () (varInfo &var){
      return *(has(var));
    }

    smntDepInfo* depMap_t::has(statement &s){
      smntToVarDepMapIterator it = s2vdMap.find(&s);

      if(it == s2vdMap.end())
        return NULL;

      return (it->second);
    }

    smntDepInfo& depMap_t::operator () (statement &s){
      return *(has(s));
    }

    varDepInfo* depMap_t::has(statement &s, varInfo &var){
      smntToVarDepMapIterator it = s2vdMap.find(&s);

      if(it == s2vdMap.end())
        return NULL;

      return (it->second->has(var));
    }

    varDepInfo& depMap_t::operator () (statement &s, varInfo &var){
      return *(has(s,var));
    }
    //============================================
  };
};
