require 'open3'

describe "sclpl a-normal form" do
  context "literals" do
    it "strings should remain untouched" do
      expect(anf('"foo"')).to eq(['T_STRING:"foo"'])
    end

    it "characters should remain untouched" do
      expect(anf('\\c')).to eq(['T_CHAR:c'])
    end

    it "integers should remain untouched" do
      expect(anf('123')).to eq(['T_INT:123'])
    end

    it "floats should remain untouched" do
      expect(anf('123.0')).to eq(['T_FLOAT:123.000000'])
    end

    it "booleans should remain untouched" do
      expect(anf('true')).to eq(['T_BOOL:true'])
    end

    it "ids should remain untouched" do
      expect(anf('foo')).to eq(['T_ID:foo'])
    end
  end

  context "definitions" do
    it "should leave atomic defintions alone" do
      expect(anf('def foo 123;')).to eq([["def", "foo", "T_INT:123"]])
    end

    it "should leave normalize complex defintions" do
      expect(anf('def foo bar();')).to eq([["def", "foo", ["T_ID:bar"]]])
    end
  end

  context "function applications" do
    it "should leave an application with no args alone" do
      expect(anf('foo()')).to eq([['T_ID:foo']])
    end

    it "should leave an application with one arg alone" do
      expect(anf('foo(a)')).to eq([['T_ID:foo', 'T_ID:a']])
    end

    it "should normalize an application with a complex function" do
      expect(anf('(foo())()')).to eq([['let', ['$:0', ['T_ID:foo']], ['$:0']]])
    end

    it "should normalize an application with a complex arg" do
      expect(anf('foo(bar())')).to eq([['let', ['$:0', ['T_ID:bar']], ['T_ID:foo', '$:0']]])
    end

    it "should normalize an application with two complex args" do
      expect(anf('foo(bar(),baz())')).to eq([
          ['let', ['$:0', ['T_ID:bar']],
            ['let', ['$:1', ['T_ID:baz']],
              ['T_ID:foo', '$:0', '$:1']]]])
    end

    it "should normalize an application with three complex args" do
      expect(anf('foo(bar(),baz(),boo())')).to eq([
          ['let', ['$:0', ['T_ID:bar']],
            ['let', ['$:1', ['T_ID:baz']],
              ['let', ['$:2', ['T_ID:boo']],
                ['T_ID:foo', '$:0', '$:1', '$:2']]]]])
    end

    it "should normalize an application with simple and complex args (s,c,c)" do
      expect(anf('foo(a,bar(),baz())')).to eq([
          ['let', ['$:0', ['T_ID:bar']],
            ['let', ['$:1', ['T_ID:baz']],
              ['T_ID:foo', 'T_ID:a', '$:0', '$:1']]]])
    end

    it "should normalize an application with simple and complex args (c,s,c)" do
      expect(anf('foo(bar(),a,baz())')).to eq([
          ['let', ['$:0', ['T_ID:bar']],
            ['let', ['$:1', ['T_ID:baz']],
              ['T_ID:foo', '$:0', 'T_ID:a', '$:1']]]])
    end

    it "should normalize an application with simple and complex args (c,c,s)" do
      expect(anf('foo(bar(),baz(),a)')).to eq([
          ['let', ['$:0', ['T_ID:bar']],
            ['let', ['$:1', ['T_ID:baz']],
              ['T_ID:foo', '$:0', '$:1', 'T_ID:a']]]])
    end
  end

  context "if expressions" do
    it "should leave atomic expressions alone" do
      expect(anf('if 1 2 3;')).to eq([
          ["if", "T_INT:1",
            ["let", ["$:0", "T_INT:2"],
              ["let", ["$:1", "T_INT:3"],
                "$:1"]]]])
    end

    it "should normalize the conditional expression" do
      expect(anf('if foo() 2 else 3;')).to eq([
          ["let", ["$:2", ["T_ID:foo"]],
            ["if", "$:2",
              ["let", ["$:0", "T_INT:2"], "$:0"],
              ["let", ["$:1", "T_INT:3"], "$:1"]]]])
    end

    it "should normalize the first branch expression" do
      expect(anf('if 1 foo() else 3;')).to eq([
        ["if", "T_INT:1",
          ["let", ["$:0", ["T_ID:foo"]], "$:0"],
          ["let", ["$:1", "T_INT:3"], "$:1"]]])
    end

    it "should normalize the first branch expression" do
      expect(anf('if 1 2 else foo();')).to eq([
        ["if", "T_INT:1",
          ["let", ["$:0", "T_INT:2"], "$:0"],
          ["let", ["$:1", ["T_ID:foo"]], "$:1"]]])
    end
  end

  context "function literals" do
    it "should normalize a literal with a complex expression" do
      expect(anf('fn() foo(bar());')).to eq([
        ["fn", [],
          ["let", ["$:1", ["T_ID:bar"]],
            ["let", ["$:0", ["T_ID:foo", "$:1"]],
                "$:0"]]]
      ])
    end
  end
end
