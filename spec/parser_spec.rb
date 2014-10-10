require 'open3'

def re_structure( token_array, offset = 0 )
  struct = []
  while( offset < token_array.length )
    if(token_array[offset] == "(")
      # Multiple assignment from the array that re_structure() returns
      offset, tmp_array = re_structure(token_array, offset + 1)
      struct << tmp_array
    elsif(token_array[offset] == ")")
      break
    else
      struct << token_array[offset]
    end
    offset += 1
  end
  return [offset, struct]
end

def ast(input)
  out, err, status = Open3.capture3('./build/bin/sclpl-test', '--ast', :stdin_data => input)
  raise err unless err == ""
  raise "Parser command returned non-zero status" unless status.success?
  #out.gsub!(/<tok (T_[A-Z]+)>/,'\1')
  out.gsub!(/([()])|tree/,' \1 ')
  off, expr = re_structure(out.split)
  expr
end

describe "sclpl grammar" do
  context "requires" do
    it "should parse a require statement" do
      expect(ast('require foo;')).to eq([ ['T_VAR:require', 'T_VAR:foo'] ])
    end

    it "should parse a require statement using end keyword" do
        expect(ast('require foo end')).to eq([ ['T_VAR:require', 'T_VAR:foo'] ])
    end

    it "should error on missing semicolon" do
      expect{ast('require foo')}.to raise_error /Invalid Syntax/
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

  context "definitions" do
    it "should parse a value definition" do
      expect(ast('def foo 123;')).to eq([ ['T_VAR:def', 'T_VAR:foo', 'T_INT:123'] ])
    end

    it "should parse a function definition" do
      expect(ast('def foo() 123;')).to eq([
        ['T_VAR:def', 'T_VAR:foo', ['T_VAR:fn', [], 'T_INT:123']] ])
    end

    it "should parse a function definition  with multiple expressions in the body" do
      expect(ast('def foo() 123 321;')).to eq([
        ['T_VAR:def', 'T_VAR:foo', ['T_VAR:fn', [], 'T_INT:123', 'T_INT:321']] ])
    end

    it "should parse a function definition with one argument" do
      expect(ast('def foo(a) 123;')).to eq([
        ['T_VAR:def', 'T_VAR:foo', ['T_VAR:fn', ['T_VAR:a'], 'T_INT:123']] ])
    end

    it "should parse a function definition with two arguments" do
      expect(ast('def foo(a,b) 123;')).to eq([
        ['T_VAR:def', 'T_VAR:foo', ['T_VAR:fn', ['T_VAR:a', 'T_VAR:b'], 'T_INT:123']] ])
    end

    it "should parse a function definition with three arguments" do
      expect(ast('def foo(a,b,c) 123;')).to eq([
        ['T_VAR:def', 'T_VAR:foo', ['T_VAR:fn', ['T_VAR:a', 'T_VAR:b', 'T_VAR:c'], 'T_INT:123']] ])
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
        expect(ast('if 123 321 end')).to eq([["T_VAR:if", "T_INT:123", "T_INT:321"]])
      end

      it "should parse an if statement with an else clause " do
        expect(ast('if 123 321 else 456 end')).to eq([
          ["T_VAR:if", "T_INT:123", "T_INT:321", "T_VAR:else", "T_INT:456"]])
      end
    end

    context "function literals" do
      it "should parse a function with no params" do
        expect(ast('fn() 123;')).to eq([["T_VAR:fn", [], "T_INT:123"]])
      end

      it "should parse a function with one param" do
        expect(ast('fn(a) 123;')).to eq([["T_VAR:fn", ["T_VAR:a"], "T_INT:123"]])
      end

      it "should parse a function with two params" do
        expect(ast('fn(a,b) 123;')).to eq([["T_VAR:fn", ["T_VAR:a", "T_VAR:b"], "T_INT:123"]])
      end
    end

    context "function application" do
      it "should parse an application with no params " do
        expect(ast('foo()')).to eq([["T_VAR:foo"]])
      end

      it "should parse an application with one param" do
        expect(ast('foo(a)')).to eq([["T_VAR:foo", "T_VAR:a"]])
      end

      it "should parse an application with two params" do
        expect(ast('foo(a,b)')).to eq([["T_VAR:foo", "T_VAR:a", "T_VAR:b"]])
      end
    end
  end

  context "literals" do
    it "should parse a string" do
      pending "Waiting for implementation of string literals"
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
      expect(ast('foo')).to eq(['T_VAR:foo'])
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
