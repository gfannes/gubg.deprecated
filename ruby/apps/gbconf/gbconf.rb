require("rubygems")
require("wx")
include Wx

class GbConfFrame < Frame
    def initialize
        super(nil, :title => "GbConf", :pos => [150, 25], :size => [300, 300])

        panel = Panel.new(self)

        @item = StaticText.new(panel, :label => "Item", :pos => [150, 25])  

        @tree = TreeCtrl.new(panel, pos: [0, 0], size: [100, 300], name: "Items")
        @root = @tree.add_root("Root")
        @tree.append_item(@root, "frutter1")
        @tree.append_item(@root, "frutter2")
        @tree.append_item(@root, "frutter3")
        @tree.append_item(@root, "frutter4")
        evt_tree_sel_changed(@tree.get_id){|evt|@item.label = @tree.get_item_text(evt.get_item)}

        @exportButton = Button.new(panel, pos: [150, 250], label: "Export")
        evt_button(@exportButton.get_id){|evt|exportTree(evt)}

        show()
    end

    #Event handlers
    def exportTree(evt)
        @tree.each do |id|
            puts("id: #{id} => #{@tree.get_item_text(id)}")
        end
    end

end

class GbConfApp < App
    def on_init
        GbConfFrame.new
    end
end

GbConfApp.new.main_loop
