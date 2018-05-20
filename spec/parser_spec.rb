require 'open3'

describe "sclpl grammar" do
  context "requires" do
    it "should parse a require statement" do
      expect(ast('require "foo";')).to eq([ ['require', '"foo"'] ])
    end

    it "should parse a require statement using end keyword" do
        expect(ast('require "foo" end')).to eq([ ['require', '"foo"'] ])
    end

    it "should error on missing semicolon" do
      expect{ast('require "foo"')}.to raise_error /Error/
    end

    it "should error on missing filename" do
      expect{ast('require ;')}.to raise_error /Error/
    end

    it "should error on invalid filename type" do
      expect{ast('require 123;')}.to raise_error /Error/
    end

    it "should error on too many parameters" do
      expect{ast('require foo bar;')}.to raise_error /Error/
    end
  end

  context "definitions" do
    it "should parse a value definition with type annotation" do
      expect(ast('def foo int 123;')).to eq([ ['def', 'foo', 'T_INT:123'] ])
    end

    it "should parse a function definition with return type annotation" do
      expect(ast('def foo() int 123;')).to eq([
        ['def', 'foo', ['fn', [],
          ["let", ["$:0", "T_INT:123"], "$:0"]]] ])
    end

    it "should error on a function definition with missing return type" do
      pending("TODO: fix the error message here")
      expect(ast('def foo() 123;')).to raise_error /Error/
    end

    it "should parse a function definition  with multiple expressions in the body" do
      expect(ast('def foo() int 123 321;')).to eq([
        ['def', 'foo', ['fn', [],
          ["let", ["$:0", "T_INT:123"],
            ["let", ["$:1", "T_INT:321"], "$:1"]]]]])
    end

    it "should parse a function definition with one definition and expression" do
      expect(ast('def foo() int def bar int 123; add(bar,321);')).to eq([
        ['def', 'foo', ['fn', [],
          ["let", ["T_ID:bar", "T_INT:123"],
            ["let", ["$:0", ["T_ID:add", "T_ID:bar", "T_INT:321"]], "$:0"]]]]])
    end

    it "should parse a function definition with one argument" do
      expect(ast('def foo(a int) int 123;')).to eq([
        ['def', 'foo', ['fn', ['T_ID:a'],
          ["let", ["$:0", "T_INT:123"], "$:0"]]]])
    end

    it "should parse a function definition with two arguments" do
      expect(ast('def foo(a int, b int) int 123;')).to eq([
        ['def', 'foo', ['fn', ['T_ID:a', 'T_ID:b'],
          ["let", ["$:0", "T_INT:123"], "$:0"]]]])
    end

    it "should parse a function definition with three arguments" do
      expect(ast('def foo(a int, b int, c int) int 123;')).to eq([
        ['def', 'foo', ['fn', ['T_ID:a', 'T_ID:b', 'T_ID:c'],
          ["let", ["$:0", "T_INT:123"], "$:0"]]]])
    end
  end

#  context "literals" do
#    it "should parse a string" do
#      expect(ast('"foo"')).to eq(['T_STRING:"foo"'])
#    end
#
#    it "should parse a character" do
#      expect(ast('\\c')).to eq(['T_CHAR:c'])
#    end
#
#    it "should parse an integer" do
#      expect(ast('123')).to eq(['T_INT:123'])
#    end
#
#    it "should parse a float" do
#      expect(ast('123.0')).to eq(['T_FLOAT:123.000000'])
#    end
#
#    it "should parse boolean" do
#      expect(ast('true')).to eq(['T_BOOL:true'])
#    end
#
#    it "should parse an identifier" do
#      expect(ast('foo')).to eq(['T_ID:foo'])
#    end
#  end
#
#  context "if statements" do
#    it "should parse an if statement with no else clause" do
#      expect(ast('if 123 321 end')).to eq([
#        ["if", "T_INT:123", ["let", ["$:0", "T_INT:321"], "$:0"]]])
#    end
#
#    it "should parse an if statement with an optional then keyword" do
#      expect(ast('if 123 then 321 end')).to eq([
#        ["if", "T_INT:123", ["let", ["$:0", "T_INT:321"], "$:0"]]])
#    end
#
#    it "should parse an if statement with an else clause " do
#      expect(ast('if 123 321 else 456 end')).to eq([
#        ["if", "T_INT:123",
#          ["let", ["$:0", "T_INT:321"], "$:0"],
#          ["let", ["$:1", "T_INT:456"], "$:1"]]])
#    end
#
#    it "should parse an if statement with a then clause with multiple expressions" do
#      expect(ast('if 1 2 3 else 4 end')).to eq([
#        ["if", "T_INT:1",
#          ["let", ["$:0", "T_INT:2"],
#            ["let", ["$:1", "T_INT:3"], "$:1"]],
#          ["let", ["$:2", "T_INT:4"], "$:2"]]])
#    end
#
#    it "should parse an if statement with an else clause with multiple expressions" do
#      expect(ast('if 1 2 else 3 4 end')).to eq([
#        ["if", "T_INT:1",
#          ["let", ["$:0", "T_INT:2"], "$:0"],
#          ["let", ["$:1", "T_INT:3"],
#            ["let", ["$:2", "T_INT:4"], "$:2"]],
#      ]])
#    end
#  end

#  context "expressions" do
#    context "parenthese grouping" do
#      it "should parse a parenthesized expression" do
#        expect(ast('(123)')).to eq(['T_INT:123'])
#      end
#
#      it "should parse a nested parenthesized expression" do
#        expect(ast('((123))')).to eq(['T_INT:123'])
#      end
#    end
#
#    context "function literals" do
#      it "should parse a function with no params" do
#        expect(ast('fn() 123;')).to eq([
#          ["fn", [],
#            ["let", ["$:0", "T_INT:123"], "$:0"]]])
#      end
#
#      it "should parse a function with no params and a return type annotation" do
#        expect(ast('fn():int 123;')).to eq([
#          ["fn", [],
#            ["let", ["$:0", "T_INT:123"], "$:0"]]])
#      end
#
#      it "should parse a function with one param" do
#        expect(ast('fn(a) 123;')).to eq([
#          ["fn", ["T_ID:a"],
#            ["let", ["$:0", "T_INT:123"], "$:0"]]])
#      end
#
#      it "should parse a function with one param with type annotation" do
#        expect(ast('fn(a:int) 123;')).to eq([
#          ["fn", ["T_ID:a"],
#            ["let", ["$:0", "T_INT:123"], "$:0"]]])
#      end
#
#      it "should parse a function with two params" do
#        expect(ast('fn(a,b) 123;')).to eq([
#          ["fn", ["T_ID:a", "T_ID:b"],
#            ["let", ["$:0", "T_INT:123"], "$:0"]]])
#      end
#
#      it "should parse a literal with two sequential simple expressions" do
#        expect(ast('fn() 1 2;')).to eq([
#          ["fn", [],
#            ["let", ["$:0", "T_INT:1"],
#              ["let", ["$:1", "T_INT:2"],
#                "$:1"]]]
#        ])
#      end
#
#      it "should parse a literal with three sequential simple expressions" do
#        expect(ast('fn() 1 2 3;')).to eq([
#          ["fn", [],
#            ["let", ["$:0", "T_INT:1"],
#              ["let", ["$:1", "T_INT:2"],
#                ["let", ["$:2", "T_INT:3"],
#                  "$:2"]]]]
#        ])
#      end
#
#      it "should parse a literal with a complex expression" do
#        expect(ast('fn() foo(bar());')).to eq([
#          ["fn", [], ["let", ["$:0", ["T_ID:foo", ["T_ID:bar"]]], "$:0"]]
#        ])
#      end
#
#      it "should normalize a literal with an if expression" do
#        expect(ast('fn() if 1 2 else 3;;')).to eq([
#          ["fn", [],
#            ["let", ["$:0", ["if", "T_INT:1",
#                              ["let", ["$:1", "T_INT:2"], "$:1"],
#                              ["let", ["$:2", "T_INT:3"], "$:2"]]],
#              "$:0"]]
#        ])
#      end
#
#      it "should normalize a literal with two sequential if expressions" do
#        expect(ast('fn() if 1 2 else 3; if 1 2 else 3; ;')).to eq([
#          ["fn", [],
#            ["let", ["$:0", ["if", "T_INT:1",
#                              ["let", ["$:1", "T_INT:2"], "$:1"],
#                              ["let", ["$:2", "T_INT:3"], "$:2"]]],
#              ["let", ["$:3", ["if", "T_INT:1",
#                                ["let", ["$:4", "T_INT:2"], "$:4"],
#                                ["let", ["$:5", "T_INT:3"], "$:5"]]],
#                "$:3"]]]
#        ])
#      end
#
#    end
#
#    context "function application" do
#      it "should parse an application with no params " do
#        expect(ast('foo()')).to eq([["T_ID:foo"]])
#      end
#
#      it "should parse an application with one param" do
#        expect(ast('foo(a)')).to eq([["T_ID:foo", "T_ID:a"]])
#      end
#
#      it "should parse an application with two params" do
#        expect(ast('foo(a,b)')).to eq([["T_ID:foo", "T_ID:a", "T_ID:b"]])
#      end
#    end
#  end
#
#  context "corner cases" do
#    it "an unexpected terminator should error" do
#      expect{ast(';')}.to raise_error /Error/
#    end
#
#    it "an invalid literal should error" do
#      expect{ast('\'')}.to raise_error /Error/
#    end
#  end

end
