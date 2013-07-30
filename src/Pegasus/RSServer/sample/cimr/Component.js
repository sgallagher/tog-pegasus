dojo.require("dijit.dijit");
dojo.require("dojox.data.JsonRestStore");
dojo.require("dojox.grid.DataGrid");
dojo.require("dojo.fx");
dojo.require("dojox.timing");
dojo.require("dojox.layout.ScrollPane");

dojo.require("dojox.charting.Chart2D");
dojo.require("dojox.charting.themes.PlotKit.orange");
dojo.require("dojox.charting.widget.Legend");


dojo.provide("cimr.Component");
dojo.provide("cimr.Processes");

dojo.declare("cimr.Component", null, {
    constructor: function(args) {
        this.host = args.host;
        //this.base_uri = "http://" + args.host + ":5988" + args.uri;
        this.base_uri = "http://localhost/cimrs/" + args.host + args.uri;
        this.id = dijit.getUniqueId(this.declaredClass.replace(/\./g,"_"));

        this.store = new dojox.data.JsonRestStore({
            target: this.base_uri
        });

        this.timer = new dojox.timing.Timer(5000);
        this.timer.onTick = this.onTick;
        this.timer.start();
    },

    onTick: function() {}


});

dojo.declare("cimr.KeyVal", cimr.Component, {
    constructor: function(args, node) {
        var containerNode = document.createElement("div");
        this.contentsNode = document.createElement("ol");

        this.store.fetch({
            onItem: dojo.hitch(this, function(item) {
                var i = 0;
                try {
                    dojo.forEach(this.store.getAttributes(item), dojo.hitch(this, function(attr) {
                        if (attr == "_loadObject") {
                            return;
                        }

                        var itemContainer = document.createElement("li");
                        var keyNode = document.createElement("div");
                        keyNode.textContent = attr;
                        dojo.addClass(keyNode, "key");

                        var valueNode = document.createElement("div");
                        valueNode.textContent = this.store.getValue(item, attr);
                        dojo.addClass(valueNode, "value");
                        itemContainer.appendChild(valueNode);
                        itemContainer.appendChild(keyNode);


                        if (++i % 2 == 0)
                            dojo.addClass(itemContainer, "even");
                        this.contentsNode.appendChild(itemContainer);
                    }));
                } catch(err) {
                    console.debug(err);
                }
            }),
            onComplete: dojo.hitch(this, function() {
                console.debug("complete!");
                try {
                    containerNode.appendChild(this.contentsNode);
                    node.appendChild(containerNode);

                    this.container = new dojox.layout.ScrollPane({
                        orientation: "vertical",
                        style:"width:99%; height:200px;"
                    }, containerNode);

                    this.container.startup();
                } catch (err) {
                    console.debug(err);
                }
            })
        });
    }

});

dojo.declare("cimr.Chart", cimr.Component, {
    constructor: function(args, node) {
        console.debug("Entered cimr.Chart constructor");
        var containerNode = document.createElement("div");
        containerNode.style.height = "180px";
        containerNode.style.width = "330px";
        node.appendChild(containerNode);

        this.chart = new dojox.charting.Chart2D(containerNode);
        if (args.theme) {
            dojo.require(args.theme);
            args.theme = eval(args.theme);
        } else {
            args.theme = dojox.charting.themes.PlotKit.orange;
        }
        this.chart.setTheme(args.theme);

        this.fields = [];
        this.node = node;
    }
});

dojo.declare("cimr.PieChart", cimr.Chart, {
    constructor: function(args, node) {
        console.debug("Entered cimr.PieChart constructor");
        this.chart.addPlot("default", {type: "Pie", radius: 60, fontColor: "black", labelOffset: -20});
        this.data = [];
        this.total = args.total;

        for (var i = 0; i < args.fields.length; i++) {
            this.data.push(100 / args.fields.length);
            this.fields.push(args.fields[i]);
        }
        this.chart.addSeries(args.name, this.data);

        this.chart.render();

        this.timer.onTick = dojo.hitch(this, this.updateChart);
        this.timer.setInterval(3000);
        this.timer.stop(); this.timer.start();

        console.debug("Adding Legend Node...");
        var legendNode = document.createElement("div");
        this.legend = new dojox.charting.widget.Legend({
            chartRef: this
        }, legendNode);
        this.node.appendChild(this.legend.domNode);
        this.legend.startup();
        console.debug("Legend Node added.");
    },

    updateChart: function() {
        this.store.fetch({
            onComplete: dojo.hitch(this, function(items) {
                var item = items[0];

                try {


                    for (var i = 0; i < this.fields.length; i++) {
                        var field = this.fields[i];
                        this.data[i] = ( this.store.getValue(item, field) / this.store.getValue(item, this.total) );


                        console.debug(this.data);
                        this.chart.updateSeries(field, this.data[i]);
                    }

                    this.chart.render();
                } catch (err) {
                    console.debug(err);
                }
            })
        });

    }
});

dojo.declare("cimr.TimeChart", cimr.Chart, {
    constructor: function(args, node) {
        this.x = 1;

        this.chart.addAxis("x", {fixLower: "minor", natural: true });
        this.chart.addAxis("y", {vertical: true });
        this.chart.addPlot("default", {type: "Markers"});

        for (var i = 0; i < args.fields.length; i++) {
            var field = { name: args.fields[i], data: [] };
            this.fields.push(field);
            this.chart.addSeries(field.name, field.data);
        }

        this.timer.onTick = dojo.hitch(this, this.updateChart);
        this.timer.setInterval(1000);
        this.timer.stop(); this.timer.start();
    },

    updateChart: function() {
        this.x++;
        this.store.fetch({
            onComplete: dojo.hitch(this, function(items) {
                var item = items[0];

                try {
                    for (var i = 0; i < this.fields.length; i++) {
                        var field = this.fields[i];
                        if (field.data.length > 40)
                            field.data.shift();
                        field.data.push({ x: this.x, y: this.store.getValue(item, field.name) });
                        this.chart.updateSeries(field.name, field.data);
                    }

                    this.chart.render();
                } catch (err) {
                    console.debug(err);
                }

                if (this.x == 2) {
                    console.debug("Adding Legend Node...");
                    var legendNode = document.createElement("div");
                    this.legend = new dojox.charting.widget.Legend({
                        chartRef: this
                    }, legendNode);
                    this.node.appendChild(this.legend.domNode);
                    this.legend.startup();
                    console.debug("Legend Node added.");
                }
            })
        });

    }
});


dojo.declare("cimr.Processes", cimr.Component, {
    constructor: function(args, node) {
        console.debug("Proccesses constructor called for " + this.host + ", id " + this.id);
        var controlNode = document.createElement("div");
        var detailsNode = document.createElement("a");
        //var closeNode = document.createElement("a");
        detailsNode.setAttribute("href", "#");
        detailsNode.textContent = "details";
        detailsNode.onclick = dojo.hitch(this, this.showDetails);
        /*closeNode.setAttribute("href", "#");
        closeNode.textContent = "close";
        closeNode.onclick = function() {
            console.debug("close!");
        }*/
        controlNode.appendChild(detailsNode);
        //controlNode.appendChild(document.createTextNode(" | "));
        //controlNode.appendChild(closeNode);
        dojo.addClass(controlNode, "controlNode");
        node.childNodes[0].insertBefore(controlNode, node.childNodes[0].childNodes[0]);

        try {
            dojo.addClass(node, "gridContainer");
            this.contentsNode = document.createElement("div");
            this.contentsNode.opacity = 0;



            this.gridLayout = [
                { field: 'Caption', name: 'Caption', width: '80px' },
                { field: 'Handle', name: 'Handle', width: '35px' },
                { field: 'ProcessNiceValue', name: 'Nice', width: '30px' },
                { field: 'UserModeTime', name: 'User Time', width: '40px' },
                { field: 'KernelModeTime', name: 'Kernel Time', width: '40px' }
            ];
            this.grid = new dojox.grid.DataGrid({
                id: this.id,
                store: this.store,
                structure: this.gridLayout,
                //rowsPerPage: 16,
                rowSelector: "15px",
                clientSort: true
            }, this.contentsNode);

            node.appendChild(this.grid.domNode);
            this.grid.startup();
            this.contentsNode.opacity = 0;
            setTimeout(dojo.hitch(this, function() {
                dojo.fadeIn({
                    node: this.id,
                    duration: 1500
                }).play();
            }), 500);

            //this.timer.onTick = dojo.hitch(this, function() { this.grid.filter() });
        } catch(err) {
            console.debug(err);
        }
    },

    showDetails: function() {

    }

});
