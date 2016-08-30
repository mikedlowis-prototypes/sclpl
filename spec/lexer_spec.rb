require 'spec_helper'

describe "lexer" do
  context "punctuation" do
    it "should recognize [" do
      expect(lexer('[')).to eq ['T_LBRACK']
    end

    it "should recognize ]" do
      expect(lexer(']')).to eq ['T_RBRACK']
    end

    it "should recognize (" do
      expect(lexer('(')).to eq ['T_LPAR']
    end

    it "should recognize )" do
      expect(lexer(')')).to eq ['T_RPAR']
    end

    it "should recognize {" do
      expect(lexer('{')).to eq ['T_LBRACE']
    end

    it "should recognize }" do
      expect(lexer('}')).to eq ['T_RBRACE']
    end

    it "should recognize '" do
      expect(lexer('\'')).to eq ['T_SQUOTE']
    end

    it "should recognize ," do
      expect(lexer(',')).to eq ['T_COMMA']
    end

    it "should recognize ;" do
      expect(lexer(';')).to eq ['T_END']
    end

    it "should recognize :" do
      expect(lexer(':')).to eq ['T_COLON']
    end

    it "should recognize all punctuation" do
      expect(lexer('[](){}\',;')).to eq(
        ["T_LBRACK", "T_RBRACK", "T_LPAR", "T_RPAR", "T_LBRACE", "T_RBRACE",
         "T_SQUOTE", "T_COMMA", "T_END"])
    end

    it "should recognize [ after an identifier" do
      expect(lexer('foo[')).to eq(['T_ID:foo', 'T_LBRACK'])
    end

    it "should recognize ] after an identifier" do
      expect(lexer('foo]')).to eq(['T_ID:foo', 'T_RBRACK'])
    end

    it "should recognize ( after an identifier" do
      expect(lexer('foo(')).to eq(['T_ID:foo', 'T_LPAR'])
    end

    it "should recognize ) after an identifier" do
      expect(lexer('foo)')).to eq(['T_ID:foo', 'T_RPAR'])
    end

    it "should recognize { after an identifier" do
      expect(lexer('foo{')).to eq(['T_ID:foo', 'T_LBRACE'])
    end

    it "should recognize } after an identifier" do
      expect(lexer('foo}')).to eq(['T_ID:foo', 'T_RBRACE'])
    end

    it "should recognize } after an identifier" do
      expect(lexer('foo\'')).to eq(['T_ID:foo', 'T_SQUOTE'])
    end

    it "should recognize } after an identifier" do
      expect(lexer('foo,')).to eq(['T_ID:foo', 'T_COMMA'])
    end

    it "should recognize } after an identifier" do
      expect(lexer('foo;')).to eq(['T_ID:foo', 'T_END'])
    end
  end

  context "characters" do
    it "should recognize space" do
      expect(lexer('\space')).to eq ['T_CHAR:\space']
    end

    it "should recognize newline" do
      expect(lexer('\newline')).to eq ['T_CHAR:\newline']
    end

    it "should recognize return" do
      expect(lexer('\return')).to eq ['T_CHAR:\return']
    end

    it "should recognize tab" do
      expect(lexer('\tab')).to eq ['T_CHAR:\tab']
    end

    it "should recognize vertical tab" do
      expect(lexer('\vtab')).to eq ['T_CHAR:\vtab']
    end

    it "should recognize 'c'" do
      expect(lexer('\c')).to eq ['T_CHAR:\c']
    end

    it "should recognize invalid named characters as identifiers" do
      expect(lexer('\foobar')).to eq ['T_ID:\foobar']
    end
  end

  context "numbers" do
    context "integers" do
      it "should recognize positive integer without sign" do
        expect(lexer('123')).to eq ['T_INT:123']
      end

      it "should recognize positive integer with sign" do
        expect(lexer('+123')).to eq ['T_INT:123']
      end

      it "should recognize negitve integer with sign" do
        expect(lexer('-123')).to eq ['T_INT:-123']
      end

      it "should recognize invalid ints as identifiers" do
        expect(lexer('123a')).to eq ['T_ID:123a']
      end
    end

    context "radix integers" do
      it "should recognize binary integer" do
        expect(lexer('0b101')).to eq ['T_INT:5']
      end

      it "should recognize octal integer" do
        expect(lexer('0o707')).to eq ['T_INT:455']
      end

      it "should recognize decimal integer" do
        expect(lexer('0d909')).to eq ['T_INT:909']
      end

      it "should recognize decimal integer" do
        expect(lexer('0hf0f')).to eq ['T_INT:3855']
      end

      it "should recognize invalid radix ints as identifiers" do
        expect(lexer('0b012')).to eq ['T_ID:0b012']
      end
    end

    context "floating point" do
      it "should recognize positive float without sign" do
        expect(lexer('123.0')).to eq ['T_FLOAT:123.000000']
      end

      it "should recognize positive float with sign" do
        expect(lexer('+123.0')).to eq ['T_FLOAT:123.000000']
      end

      it "should recognize negative float with sign" do
        expect(lexer('-123.0')).to eq ['T_FLOAT:-123.000000']
      end

      it "should recognize invalid floats as identifiers" do
        expect(lexer('123..0')).to eq ['T_ID:123..0']
      end
    end
  end

  context "boolean" do
    it "should recognize true" do
      expect(lexer('true')).to eq ['T_BOOL:true']
    end

    it "should recognize false" do
      expect(lexer('false')).to eq ['T_BOOL:false']
    end
  end

  context "identifiers" do
    it "should recognize an identifier" do
      expect(lexer('foo')).to eq ['T_ID:foo']
    end
  end

  context "strings" do
    it "should recognize an empty string" do
      expect(lexer('""')).to eq ['T_STRING:""']
    end

    it "should recognize a string with one element" do
      expect(lexer('"a"')).to eq ['T_STRING:"a"']
    end

    it "should recognize a string with two elements" do
      expect(lexer('"ab"')).to eq ['T_STRING:"ab"']
    end

    it "should recognize a string with a space" do
      expect(lexer('"a b"')).to eq ['T_STRING:"a b"']
    end

    it "should recognize a string that spans lines" do
      expect(lexer("\"a\nb\"")).to eq ["T_STRING:\"a\nb\""]
    end

    it "should recognize larger strings" do
      expect(lexer("\"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\"")).to eq [
          "T_STRING:\"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\""]
    end

    #it "should raise an assertion exception when the file ends before a string terminates" do
    #  expect{lexer("\"abc")}.to raise_error(/AssertionException/)
    #end
  end
end

