require 'open3'

describe "sclpl a-normal form" do
  context "literals" do
    it "strings should remain untouched" do
      expect(ast('"foo"')).to eq(['T_STRING:"foo"'])
    end

    it "characters should remain untouched" do
      expect(ast('\\c')).to eq(['T_CHAR:c'])
    end

    it "integers should remain untouched" do
      expect(ast('123')).to eq(['T_INT:123'])
    end

    it "floats should remain untouched" do
      expect(ast('123.0')).to eq(['T_FLOAT:123.000000'])
    end

    it "booleans should remain untouched" do
      expect(ast('true')).to eq(['T_BOOL:true'])
    end

    it "ids should remain untouched" do
      expect(ast('foo')).to eq(['T_ID:foo'])
    end
  end
end
