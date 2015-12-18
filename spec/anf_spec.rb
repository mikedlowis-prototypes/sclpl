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
  end
end
