dojo.require("dijit.layout.ContentPane");
dojo.require("dojo.dnd.Source");

dojo.provide("cimr.SystemContainer");

dojo.declare("cimr.SystemContainer", dijit.layout.ContentPane, {

    preamble: function(args, node) {
        this.title = args.host;
        this.components = [];
    },

    postCreate: function() {
        this.startup();

        this.sourceNode = document.createElement("div");
        this.domNode.appendChild(this.sourceNode);

        this.source = new dojo.dnd.Source(this.sourceNode, {
            creator: this.componentNodeCreator,
            accept: [ "component" ],
            copyOnly: false
        });
    },

    addComponents: function(/*Array*/components) {
        this.components.push(components);

        for (var i = 0; i < components.length; i++) {
            components[i]['desc'] += "<strong>" + this.host + "</strong>";
            components[i]['host'] = this.host;
        }
        this.source.insertNodes(false, components);
    },

    componentNodeCreator: function(item, hint) {
        var container = new dijit.layout.ContentPane({}, document.createElement('div'));
        dojo.addClass(container.domNode, "componentContainer");
        dojo.attr(container.domNode, "dndtype", "component");

        var handle = document.createElement("div");
        dojo.addClass(handle, "dojoDndHandle");
        dojo.addClass(handle, "GcDndHandle");

        var img = document.createElement("img");
        img.src = "cimr/images/" + item.name + ".png";
        handle.appendChild(img);

        var titleNode = document.createElement("h1");
        titleNode.textContent = item.name;
        handle.appendChild(titleNode);

        var descNode = document.createElement('div');

        dojo.addClass(descNode, "description");
        descNode.innerHTML = item.desc;
        container.domNode.appendChild(handle);
        container.domNode.appendChild(descNode);

        return {node: container.domNode, data: item, type: [ "component" ]};
    }
});
