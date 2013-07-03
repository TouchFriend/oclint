#include "TestHeaders.h"

#include "oclint/AbstractASTVisitorRule.h"

using namespace std;
using namespace clang;
using namespace oclint;

class SuppressHelperTestASTRule : public AbstractASTVisitorRule<SuppressHelperTestASTRule>
{
public:
    virtual const string name() const
    {
        return "test ast rule";
    }

    virtual int priority() const
    {
        return 0;
    }

    bool VisitDecl(clang::Decl *decl)
    {
        addViolation(decl, this);
        return true;
    }

    bool VisitStmt(clang::Stmt *stmt)
    {
        addViolation(stmt, this);
        return true;
    }
};

TEST(SuppressHelperTestASTRuleTest, PropertyTest)
{
    SuppressHelperTestASTRule rule;
    EXPECT_EQ(0, rule.priority());
    EXPECT_EQ("test ast rule", rule.name());
}

TEST(SuppressHelperTestASTRuleTest, NoSuppress)
{
    testRuleOnCode(new SuppressHelperTestASTRule(), "void a() {}", 0, 1, 1, 1, 11);
    testRuleOnCode(new SuppressHelperTestASTRule(), "void a() {}", 1, 1, 10, 1, 11);
}

TEST(SuppressHelperTestASTRuleTest, SimpleSuppressEntireMethod)
{
    testRuleOnCode(new SuppressHelperTestASTRule(),
        "void __attribute__((annotate(\"oclint:suppress[test ast rule]\"))) a() { int i = 1; }");
}

TEST(SuppressHelperTestASTRuleTest, SimpleSuppressEntireCXXClass)
{
    testRuleOnCXXCode(new SuppressHelperTestASTRule(),
        "class __attribute__((annotate(\"oclint:suppress[test ast rule]\"))) c { void a() { int i = 1; } };");
}

TEST(SuppressHelperTestASTRuleTest, CXXClassSuppressOnClass)
{
    testRuleOnCXXCode(new SuppressHelperTestASTRule(),
        "class __attribute__((annotate(\"oclint:suppress\"))) c { void a(); };\n\
void c::a() {}\n",
        1, 2, 13, 2, 14);
}

TEST(SuppressHelperTestASTRuleTest, CXXClassSuppressOnMethod)
{
    testRuleOnCXXCode(new SuppressHelperTestASTRule(),
        "class c { void a(); };\n\
void __attribute__((annotate(\"oclint:suppress\"))) c::a() { int i = 1; }\n",
        1, 1, 11, 1, 18);
}

TEST(SuppressHelperTestASTRuleTest, ObjCContainerSuppressOnAtInterface)
{
    testRuleOnObjCCode(new SuppressHelperTestASTRule(),
        "__attribute__((annotate(\"oclint:suppress\"))) @interface a {\nint i;\n}\n@end");
}

TEST(SuppressHelperTestASTRuleTest, ObjCContainerSuppressOnAtProtocol)
{
    testRuleOnObjCCode(new SuppressHelperTestASTRule(),
        "__attribute__((annotate(\"oclint:suppress\"))) @protocol a\n-(void)m;\n@end");
}

// TEST(SuppressHelperTestASTRuleTest, ObjCContainerSuppressOnAtImplementation)
// Prefix attribute must be followed by an interface or protocol,
// in other words, adding attribute to @implementation is prohibited.

TEST(SuppressHelperTestASTRuleTest, ObjCContainerSuppressOnMethod)
{
    testRuleOnObjCCode(new SuppressHelperTestASTRule(),
        "__attribute__((annotate(\"oclint:suppress\"))) @interface a {\nint i;\n}\n-(void)m __attribute__((annotate(\"oclint:suppress\")));\n@end\n\
/* __attribute__((annotate(\"oclint:suppress\"))) */ @implementation a\n-(void)m __attribute__((annotate(\"oclint:suppress\"))) {}\n@end\n",
        0, 6, 52, 8, 1);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
