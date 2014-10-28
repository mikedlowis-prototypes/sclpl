require 'open3'

describe "sclpl grammar" do
  context "requires" do
    it "should parse a require statement" do
      expect(ast('require "foo";')).to eq([ ['T_ID:require', 'T_STRING:"foo"'] ])
    end

    it "should parse a require statement using end keyword" do
        expect(ast('require "foo" end')).to eq([ ['T_ID:require', 'T_STRING:"foo"'] ])
    end

    it "should error on missing semicolon" do
      expect{ast('require "foo"')}.to raise_error /Invalid Syntax/
    end

    it "should error on missing filename" do
      expect{ast('require ;')}.to raise_error /Invalid Syntax/
    end

    it "should error on invalid filename type" do
      expect{ast('require 123;')}.to raise_error /Invalid Syntax/
    end

    it "should error on too many parameters" do
      expect{ast('require foo bar;')}.to raise_error /Invalid Syntax/
    end
  end

  context "type definitions" do
  end

  context "definitions" do
    it "should parse a value definition" do
      expect(ast('def foo 123;')).to eq([ ['T_ID:def', 'T_ID:foo', 'T_INT:123'] ])
    end

    it "should parse a function definition" do
      expect(ast('def foo() 123;')).to eq([
        ['T_ID:def', 'T_ID:foo', ['T_ID:fn', [], 'T_INT:123']] ])
    end

    it "should parse a function definition  with multiple expressions in the body" do
      expect(ast('def foo() 123 321;')).to eq([
        ['T_ID:def', 'T_ID:foo', ['T_ID:fn', [], 'T_INT:123', 'T_INT:321']] ])
    end

    it "should parse a function definition with one argument" do
      expect(ast('def foo(a) 123;')).to eq([
        ['T_ID:def', 'T_ID:foo', ['T_ID:fn', ['T_ID:a'], 'T_INT:123']] ])
    end

    it "should parse a function definition with two arguments" do
      expect(ast('def foo(a,b) 123;')).to eq([
        ['T_ID:def', 'T_ID:foo', ['T_ID:fn', ['T_ID:a', 'T_ID:b'], 'T_INT:123']] ])
    end

    it "should parse a function definition with three arguments" do
      expect(ast('def foo(a,b,c) 123;')).to eq([
        ['T_ID:def', 'T_ID:foo', ['T_ID:fn', ['T_ID:a', 'T_ID:b', 'T_ID:c'], 'T_INT:123']] ])
    end
  end

  context "annotations" do
    it "should parse a type annotation" do
      pending "Type annotations have not been implemented yet"
      expect(ast('ann foo int;')).to eq([ ['T_ID:ann', 'T_ID:foo', 'T_ID:int'] ])
    end

    it "should parse a type annotation with a generic type" do
      pending "Type annotations have not been implemented yet"
      expect(ast('ann foo Pair[int,int];')).to eq([
          ['T_ID:ann', 'T_ID:foo', ['T_ID:Pair', 'T_ID:int', 'T_ID:int']] ])
    end
  end

  context "expressions" do
    context "parenthese grouping" do
      it "should parse a parenthesized expression" do
        expect(ast('(123)')).to eq([['T_INT:123']])
      end

      it "should parse a nested parenthesized expression" do
        expect(ast('((123))')).to eq([[['T_INT:123']]])
      end
    end

    context "if statements" do
      it "should parse an if statement with no else clause" do
        expect(ast('if 123 321 end')).to eq([["T_ID:if", "T_INT:123", "T_INT:321"]])
      end

      it "should parse an if statement with an else clause " do
        expect(ast('if 123 321 else 456 end')).to eq([
          ["T_ID:if", "T_INT:123", "T_INT:321", "T_ID:else", "T_INT:456"]])
      end
    end

    context "function literals" do
      it "should parse a function with no params" do
        expect(ast('fn() 123;')).to eq([["T_ID:fn", [], "T_INT:123"]])
      end

      it "should parse a function with one param" do
        expect(ast('fn(a) 123;')).to eq([["T_ID:fn", ["T_ID:a"], "T_INT:123"]])
      end

      it "should parse a function with two params" do
        expect(ast('fn(a,b) 123;')).to eq([["T_ID:fn", ["T_ID:a", "T_ID:b"], "T_INT:123"]])
      end
    end

    context "function application" do
      it "should parse an application with no params " do
        expect(ast('foo()')).to eq([["T_ID:foo"]])
      end

      it "should parse an application with one param" do
        expect(ast('foo(a)')).to eq([["T_ID:foo", "T_ID:a"]])
      end

      it "should parse an application with two params" do
        expect(ast('foo(a,b)')).to eq([["T_ID:foo", "T_ID:a", "T_ID:b"]])
      end
    end
  end

  context "literals" do
    it "should parse a string" do
      expect(ast('"foo"')).to eq(['T_STRING:"foo"'])
    end

    it "should parse a character" do
      expect(ast('\\c')).to eq(['T_CHAR:\\c'])
    end

    it "should parse an integer" do
      expect(ast('123')).to eq(['T_INT:123'])
    end

    it "should parse a float" do
      expect(ast('123.0')).to eq(['T_FLOAT:123.000000'])
    end

    it "should parse boolean" do
      expect(ast('true')).to eq(['T_BOOL:true'])
    end

    it "should parse an identifier" do
      expect(ast('foo')).to eq(['T_ID:foo'])
    end
  end

  context "corner cases" do
    it "an unexpected terminator should error" do
      expect{ast(';')}.to raise_error /Invalid Syntax/
    end

    it "an invalid literal should error" do
      expect{ast('\'')}.to raise_error /Invalid Syntax/
    end
  end
end
