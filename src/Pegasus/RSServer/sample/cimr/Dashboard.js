dojo.require("dojox.layout.GridContainer");

dojo.provide("cimr.Dashboard");


dojo.declare("cimr.Dashboard", dojox.layout.GridContainer, {

    preamble: function(args, node) {
    },

    postCreate: function() {
        dojo.subscribe("/dnd/drop", this, "handleComponentDrop");
    },

    handleComponentDrop: function() {
        var m = dojo.dnd.manager();
        if (m.target.dom.id != this.domNode.id ||
            (m.source.node && m.source.node.parentNode.parentNode.id != "systemsContainer") ||
            (m.source.dom)) {
            return;
        }

        try {
            for (var i = 0; i < m.nodes.length; i++) {
                var node = m.nodes[i];
                var item = m.source.getItem(node.id) || m.target.getItem(node.id);
                var data = item.data;

                var obj = new data.type(data, node);
            }
        } catch (err) {
            console.debug(err);
        }
    }



});
