#ifndef OCCA_PARSER_STATEMENT_HEADER
#define OCCA_PARSER_STATEMENT_HEADER

#include "occaParserDefines.hpp"
#include "occaParserMacro.hpp"
#include "occaParserTools.hpp"
#include "occaParserNodes.hpp"
#include "occaParserTypes.hpp"

namespace occa {
  namespace parserNamespace {
    class statement;

    //---[ Exp Node ]-------------------------------
    namespace expType {
      static const int root            = (1 << 0);

      static const int LCR             = (7 << 1);
      static const int L               = (1 << 1);
      static const int C               = (1 << 2);
      static const int R               = (1 << 3);

      static const int qualifier       = (1 <<  4);
      static const int type            = (1 <<  5);
      static const int presetValue     = (1 <<  6);
      static const int operator_       = (1 <<  7);
      static const int unknown         = (1 <<  8);
      static const int variable        = (1 <<  9);
      static const int function        = (1 << 11);
      static const int functionPointer = (1 << 12);
      static const int typedef_        = (1 << 13);
      static const int prototype       = (1 << 14);
      static const int declaration     = (1 << 15);
      static const int struct_         = (1 << 16);
      static const int namespace_      = (1 << 17);
      static const int cast_           = (1 << 18);
      static const int macro_          = (1 << 19);
      static const int goto_           = (1 << 20);
      static const int gotoLabel_      = (1 << 21);
      static const int case_           = (1 << 22);
      static const int return_         = (1 << 23);
      static const int occaFor         = (1 << 24);
      static const int checkSInfo      = (1 << 25);

      static const int varInfo         = (1 << 26);
      static const int typeInfo        = (1 << 27);

      static const int printValue      = (1 << 28);
      static const int printLeaves     = (1 << 29);
      static const int maxBit          = 29;
    };

    namespace expFlag {
      static const int none        = 0;
      static const int noNewline   = (1 << 0);
      static const int noSemicolon = (1 << 1);
    }

    namespace leafType {
      static const char exp  = (1 << 0);
      static const char type = (1 << 1);
      static const char var  = (1 << 2);
    };

    class varInfo;
    class typeInfo;
    class expNode;

    class varLeaf_t {
    public:
      bool hasExp;

      varInfo *var;
      expNode *exp;

      varLeaf_t() :
        hasExp(false),

        var(NULL),
        exp(NULL) {}
    };

    class expNode {
    public:
      statement *sInfo;

      std::string value;
      int info;

      expNode *up;

      int leafCount;
      char leafInfo;

      union {
        expNode **leaves;
        typeInfo **typeLeaves;
        varLeaf_t *varLeaves;
      };

      expNode();
      expNode(statement &s);
      expNode(expNode &up_);

      inline expNode& operator [] (const int i){
        return *leaves[i];
      }

      //---[ Find Statement ]-----------
      void labelStatement(strNode *&nodeRoot);
      int getStatementType();

      int loadMacroStatement(strNode *&nodeRoot);
      int loadOccaForStatement(strNode *&nodeRoot);
      int loadTypedefStatement(strNode *&nodeRoot);
      int loadStructStatement(strNode *&nodeRoot);
      int loadUpdateStatement(strNode *&nodeRoot);
      int loadDescriptorStatement(strNode *&nodeRoot);
      int loadGotoStatement(strNode *&nodeRoot);
      int loadFlowStatement(strNode *&nodeRoot);
      int loadSpecialStatement(strNode *&nodeRoot);
      int loadBlockStatement(strNode *&nodeRoot);
      //================================

      void loadFromNode(strNode *&nodePos);

      void splitAndOrganizeNode(strNode *nodeRoot);
      void organize();

      void splitDeclareStatement(const bool addVariablesToScope = true);
      void splitFlowStatement();
      void splitFunctionStatement(const bool addVariablesToScope = true);
      void splitStructStatement(const bool addTypesToScope = true);

      void initLoadFromNode(strNode *nodeRoot);

      void initOrganization();

      void organizeLeaves();
      void organizeLeaves(const int level);

      int mergeRange(const int newLeafType,
                     const int leafPosStart,
                     const int leafPosEnd);

      // [a][::][b]
      void mergeNamespaces();

      // [(class)]
      void labelCasts();

      // const int [*] x
      void labelReferenceQualifiers();

      // [const] int x
      void mergeQualifiers();

      // [[const] [int] [*]] x
      void mergeTypes();

      // [[[const] [int] [*]] [x]]
      void mergeVariables();

      // 1 [type]                           2 [(]       3 [(]
      // [[qualifiers] [type] [qualifiers]] [(*[name])] [([args])]
      void mergeFunctionPointers();

      // class(...), class{1,2,3}
      void mergeClassConstructs();

      // static_cast<>()
      void mergeCasts();

      // [max(a,b)]
      void mergeFunctionCalls();

      void mergeArguments();

      // a[3]
      void mergeArrays();

      // (class) x
      void mergeClassCasts();

      // sizeof x
      void mergeSizeOf();

      // new, new [], delete, delete []
      void mergeNewsAndDeletes();

      // throw x
      void mergeThrows();

      // [++]i
      int mergeLeftUnary(const int leafPos);

      // i[++]
      int mergeRightUnary(const int leafPos);

      // a [+] b
      int mergeBinary(const int leafPos);

      // a [?] b : c
      int mergeTernary(const int leafPos);

      //---[ Custom Functions ]---------
      void labelNewVariables();
      //================================

      //---[ Custom Type Info ]---------
      bool qualifierEndsWithStar() const;

      bool typeEndsWithStar() const;

      bool hasAnArrayQualifier(const int pos = 0) const;
      //================================

      static void swap(expNode &a, expNode &b);

      expNode* clone(statement &s);
      expNode* clone(expNode *original);

      void cloneTo(expNode &newRoot);

      expNode* lastLeaf();

      //---[ Exp Info ]-----------------
      int depth();
      int whichLeafAmI();
      int nestedLeafCount();

      expNode* makeFlatHandle();
      void makeFlatHandle(int &offset,
                          expNode **flatLeaves);

      void addNode(const int info_, const int pos = 0);
      void addNodes(const int info_, const int pos, const int count = 1);

      varInfo& addVarInfoNode(const int pos = 0);
      typeInfo& addTypeInfoNode(const int pos = 0);

      void removeNodes(const int pos, const int count = 1);
      void removeNode(const int pos = 0);

      void convertTo(const int info_ = 0);

      bool hasQualifier(const std::string &qualifier) const;

      void addQualifier(const std::string &qualifier, const int pos = 0);
      void addPostQualifier(const std::string &qualifier, const int pos = 0);

      void removeQualifier(const std::string &qualifier);

      void changeType(const std::string &newType);

      std::string getVariableName() const;
      //================================

      void freeLeaf(const int leafPos);

      void free();

      void print(const std::string &tab = "");
      void printOn(std::ostream &out,
                   const std::string &tab = "",
                   const int flags = expFlag::none);

      std::string toString(const std::string &tab = "");
      operator std::string ();

      friend std::ostream& operator << (std::ostream &out, expNode &n);
    };

    struct statementExp {
      int sType;
      expNode exp;
    };
    //==============================================


    //---[ Statement ]------------------------------
    class statement {
    public:
      scopeTypeMap_t scopeTypeMap;
      scopeVarMap_t scopeVarMap;

      varOriginMap_t &varOriginMap;
      varUsedMap_t   &varUsedMap;

      strNode *nodeStart, *nodeEnd;

      int depth;
      statement *up;

      int type;

      expNode expRoot;

      int statementCount;
      statementNode *statementStart, *statementEnd;

      statement(parserBase &pb);

      statement(const int depth_,
                varOriginMap_t &varOriginMap_,
                varUsedMap_t &varUsedMap_);

      statement(const int depth_, statement *up_);

      statement(const int depth_,
                const int type_,
                statement *up_);

      ~statement();

      statement* makeSubStatement();

      std::string getTab() const;

      int checkMacroStatementType(strNode *&nodeRoot);
      int checkOccaForStatementType(strNode *&nodeRoot);
      int checkStructStatementType(strNode *&nodeRoot);
      int checkUpdateStatementType(strNode *&nodeRoot);
      int checkDescriptorStatementType(strNode *&nodeRoot);
      int checkGotoStatementType(strNode *&nodeRoot);
      int checkFlowStatementType(strNode *&nodeRoot);
      int checkSpecialStatementType(strNode *&nodeRoot);
      int checkBlockStatementType(strNode *&nodeRoot);

      void addType(typeInfo &type);
      void addTypedef(const std::string &typedefName);

      bool nodeHasQualifier(strNode *n) const;
      bool nodeHasSpecifier(strNode *n) const;
      bool nodeHasDescriptor(strNode *n) const;

      varInfo loadVarInfo(strNode *&nodePos);

      typeInfo* hasTypeInScope(const std::string &typeName) const;

      varInfo* hasVariableInScope(const std::string &varName) const;

      bool hasDescriptorVariable(const std::string descriptor) const;
      bool hasDescriptorVariableInScope(const std::string descriptor) const;

      void loadAllFromNode(strNode *nodeRoot);
      strNode* loadFromNode(strNode *nodeRoot);

      void setExpNodeFromStrNode(expNode &exp,
                                 strNode *nodePos);

      expNode* createExpNodeFrom(strNode *nodePos);
      expNode* createExpNodeFrom(const std::string &source);

      void loadBlocksFromLastNode(strNode *end,
                                  const int startBlockPos = 0);

      strNode* loadSimpleFromNode(const int st,
                                  strNode *nodeRoot,
                                  strNode *nodeRootEnd);

      strNode* loadForFromNode(const int st,
                               strNode *nodeRoot,
                               strNode *nodeRootEnd);

      strNode* loadWhileFromNode(const int st,
                                 strNode *nodeRoot,
                                 strNode *nodeRootEnd);

      strNode* loadIfFromNode(const int st,
                              strNode *nodeRoot,
                              strNode *nodeRootEnd);

      // [-] Missing
      strNode* loadSwitchFromNode(const int st,
                                  strNode *nodeRoot,
                                  strNode *nodeRootEnd);

      strNode* loadGotoFromNode(const int st,
                                strNode *nodeRoot,
                                strNode *nodeRootEnd);

      strNode* loadFunctionDefinitionFromNode(const int st,
                                              strNode *nodeRoot,
                                              strNode *nodeRootEnd);

      strNode* loadFunctionPrototypeFromNode(const int st,
                                             strNode *nodeRoot,
                                             strNode *nodeRootEnd);

      strNode* loadBlockFromNode(const int st,
                                 strNode *nodeRoot,
                                 strNode *nodeRootEnd);

      // [-] Missing
      strNode* loadStructFromNode(const int st,
                                  strNode *nodeRoot,
                                  strNode *nodeRootEnd);

      // [-] Missing
      strNode* loadBlankFromNode(const int st,
                                 strNode *nodeRoot,
                                 strNode *nodeRootEnd);

      // [-] Missing
      strNode* loadMacroFromNode(const int st,
                                 strNode *nodeRoot,
                                 strNode *nodeRootEnd);

      statementNode* getStatementNode();

      varInfo* addVariable(const varInfo &info,
                           statement *origin = NULL);

      void addStatement(statement *newStatement);

      statement* clone();

      void printVariablesInStatement();

      void printVariablesInScope();

      void printTypesInScope();
      void printTypesInStatement();
      void printTypeDefsInStatement();

      //---[ Statement Info ]-----------
      void swapExpWith(statement &s);

      bool hasQualifier(const std::string &qualifier) const;

      void addQualifier(const std::string &qualifier, const int pos = 0);
      void removeQualifier(const std::string &qualifier);

      expNode* getDeclarationTypeNode();
      expNode* getDeclarationVarNode(const int pos);
      std::string getDeclarationVarName(const int pos) const;
      int getDeclarationVarCount() const;

      std::string getFunctionName() const;
      void setFunctionName(const std::string &newName);
      expNode* getFunctionArgsNode();
      expNode* getFunctionArgNode(const int pos);
      std::string getFunctionArgType(const int pos);
      std::string getFunctionArgName(const int pos);
      varInfo* getFunctionArgVar(const int pos);
      int getFunctionArgCount() const;

      int getForStatementCount() const;
      //================================

      // autoMode: Handles newlines and tabs
      std::string prettyString(strNode *nodeRoot,
                               const std::string &tab_ = "",
                               const bool autoMode = true) const;

      operator std::string();
    };

    std::ostream& operator << (std::ostream &out, statement &s);
  };
};

#endif
