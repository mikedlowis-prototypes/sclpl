function lexer(input)
    return {}
end

expect = check_equal

describe "lexer" {
    context "punctuation" {
        it "should recognize [" (function()
        end),
        it "should recognize ]" (function()
        end),
    }
}

describe "lexer" {
  context "punctuation" {
    it "should recognize [" (function()
      expect(lexer('['), {"T_LBRACK"})
    end),

    it "should recognize ]" (function()
      expect(lexer(']'), {"T_RBRACK"})
    end),

    it "should recognize (" (function()
      expect(lexer('('), {"T_LPAR"})
    end),

    it "should recognize )" (function()
      expect(lexer(')'), {"T_RPAR"})
    end),

    it "should recognize {" (function()
      expect(lexer('{'), {"T_LBRACE"})
    end),

    it "should recognize }" (function()
      expect(lexer('}'), {"T_RBRACE"})
    end),

    it "should recognize '" (function()
      expect(lexer('\''), {"T_SQUOTE"})
    end),

    it "should recognize ," (function()
      expect(lexer(','), {"T_COMMA"})
    end),

    it "should recognize ;" (function()
      expect(lexer(';'), {"T_END"})
    end),

    it "should recognize all punctuation" (function()
      expect(lexer('[](){}\',;'),
        {"T_LBRACK", "T_RBRACK", "T_LPAR", "T_RPAR", "T_LBRACE", "T_RBRACE",
         "T_SQUOTE", "T_COMMA", "T_END"})
    end),

    it "should recognize [ after an identifier" (function()
      expect(lexer('foo['), {"T_ID:foo", "T_LBRACK"})
    end),

    it "should recognize ] after an identifier" (function()
      expect(lexer('foo]'), {"T_ID:foo", "T_RBRACK"})
    end),

    it "should recognize ( after an identifier" (function()
      expect(lexer("foo("), {"T_ID:foo", "T_LPAR"})
    end),

    it "should recognize ) after an identifier" (function()
      expect(lexer("foo)"), {"T_ID:foo", "T_RPAR"})
    end),

    it "should recognize { after an identifier" (function()
      expect(lexer('foo{'), {"T_ID:foo", "T_LBRACE"})
    end),

    it "should recognize } after an identifier" (function()
      expect(lexer('foo}'), {"T_ID:foo", "T_RBRACE"})
    end),

    it "should recognize } after an identifier" (function()
      expect(lexer('foo\''), {"T_ID:foo", "T_SQUOTE"})
    end),

    it "should recognize } after an identifier" (function()
      expect(lexer('foo,'), {"T_ID:foo", "T_COMMA"})
    end),

    it "should recognize } after an identifier" (function()
      expect(lexer("foo;"), {"T_ID:foo", "T_END"})
    end),
  },

  context "characters" {
    it "should recognize space" (function()
      expect(lexer("\space"), {'T_CHAR:\space'})
    end),

    it "should recognize newline" (function()
      expect(lexer("\newline"), {'T_CHAR:\newline'})
    end),

    it "should recognize return" (function()
      expect(lexer("\return"), {'T_CHAR:\return'})
    end),

    it "should recognize tab" (function()
      expect(lexer("\tab"), {'T_CHAR:\tab'})
    end),

    it "should recognize vertical tab" (function()
      expect(lexer("\vtab"), {'T_CHAR:\vtab'})
    end),

    it "should recognize 'c'" (function()
      expect(lexer("\c"), {'T_CHAR:\c'})
    end),

    it "should recognize invalid named characters as identifiers" (function()
      expect(lexer("\foobar"), {'T_ID:\foobar'})
    end),
  },

  context "numbers" {
    context "integers" {
      it "should recognize positive integer without sign" (function()
        expect(lexer("123"), {"T_INT:123"})
      end),

      it "should recognize positive integer with sign" (function()
        expect(lexer("+123"), {"T_INT:123"})
      end),

      it "should recognize negitve integer with sign" (function()
        expect(lexer("-123"), {"T_INT:-123"})
      end),

      it "should recognize invalid ints as identifiers" (function()
        expect(lexer("123a"), {"T_ID:123a"})
      end),
    },

    context "radix integers" {
      it "should recognize binary integer" (function()
        expect(lexer("0b101"), {"T_INT:5"})
      end),

      it "should recognize octal integer" (function()
        expect(lexer("0o707"), {"T_INT:455"})
      end),

      it "should recognize decimal integer" (function()
        expect(lexer("0d909"), {"T_INT:909"})
      end),

      it "should recognize decimal integer" (function()
        expect(lexer("0hf0f"), {"T_INT:3855"})
      end),

      it "should recognize invalid radix ints as identifiers" (function()
        expect(lexer("0b012"), {"T_ID:0b012"})
      end),
    },

    context "floating point" {
      it "should recognize positive float without sign" (function()
        expect(lexer("123.0"), {"T_FLOAT:123.000000"})
      end),

      it "should recognize positive float with sign" (function()
        expect(lexer("+123.0"), {"T_FLOAT:123.000000"})
      end),

      it "should recognize negative float with sign" (function()
        expect(lexer("-123.0"), {"T_FLOAT:-123.000000"})
      end),

      it "should recognize invalid floats as identifiers" (function()
        expect(lexer("123..0"), {"T_ID:123..0"})
      end),
    },
  },

  context "boolean" {
    it "should recognize true" (function()
      expect(lexer("true"), {"T_BOOL:true"})
    end),

    it "should recognize false" (function()
      expect(lexer("false"), {"T_BOOL:false"})
    end),
  },

  context "identifiers" {
    it "should recognize an identifier" (function()
      expect(lexer("foo"), {"T_ID:foo"})
    end),
  },

  context "strings" {
    it "should recognize an empty string" (function()
      expect(lexer("\"\""), {"T_STRING:\"\""})
    end),

    it "should recognize a string with one element" (function()
      expect(lexer("\"a\""), {"T_STRING:\"a\""})
    end),

    it "should recognize a string with two elements" (function()
      expect(lexer("\"ab\""), {"T_STRING:\"ab\""})
    end),

    it "should recognize a string with a space" (function()
      expect(lexer("\"a b\""), {"T_STRING:\"a b\""})
    end),

    it "should recognize a string that spans lines" (function()
      expect(lexer("\"a\nb\""), {"T_STRING:\"a\nb\""})
    end),

    it "should recognize larger strings" (function()
      expect(lexer("\"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\""), {
          "T_STRING:\"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\""})
    end),

    --it "should raise an assertion exception when the file ends before a string terminates" (function()
    --  expect{lexer("\"abc")}.to raise_error(/AssertionException/)
    --end),
  },
}
