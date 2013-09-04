
//base url which is identical for all requests
baseURL = "http://"+window.location.hostname+":"+window.location.port+"/cimrs/";
host = "http://"+window.location.hostname+":"+window.location.port+"/";
configURL = "root%2FPG_InterOp/enum?class=cim_indicationsubscription";
currentElement = null;

function insertindicationRow(info, handler, filter, state, ref) {
    var t_rows = document.getElementById('indicationTable').rows;
	

    var x=1;

    for (x=1; x < t_rows.length; x++) {
        var celValue = t_rows[x].cells[0].textContent;
        if (name < celValue) {
            break;
        }
    }

	
    var t_row = document.getElementById('indicationTable').insertRow(x);
    t_row.id = "row_" + ref;
    var t_info =t_row.insertCell(0);
    var t_handler = t_row.insertCell(1);
    var t_filter =t_row.insertCell(2);
    var t_state = t_row.insertCell(3);
	
	
	
    refArray[name] = ref;

    t_info.innerHTML=info;
    t_handler.innerHTML=handler;
    t_filter.innerHTML=filter;
    t_state.innerHTML = state;
}


/**
 * Writes received content on page.
 */
function writeContent(data) {
    //define a global variable to store property names and corresponding $ref values 
    refArray = new Array();
	
    var indicationList = JSON.parse(data);

    var i=0;
    for (i=0; i < indicationList.length; i++) {
        insertindicationRow(indicationList[i].SubscriptionInfo,
                          indicationList[i].Handler,
                          indicationList[i].Filter,
                          indicationList[i].SubscriptionState,
                          indicationList[i].$ref);
    }
    //change background color for every second row to improve readability
    var t_rows = document.getElementById('indicationTable').rows;
    var x;
    for (x=0; x < t_rows.length; x+=2) {
        document.getElementById('indicationTable').rows[x].style.background = "#EEEEEE";
        document.getElementById('indicationTable').rows[x+1].style.background = "#E0E0E0";		
    }
	
	
}

/**function to delete all rows of a table except the head row
 * afterwards a new request is send to get the lastet data and the table can be rebuilt
 */
function reloadTable(table)
{
    var rows = table.rows;
    var rowCount = rows.length;
    for (var i = rowCount-1; i > 0; i--)
        {
            table.deleteRow(i);
        }
   //default case, change it to redraw another table
   genericRequest('root%2FPG_InterOp/enum?class=cim_indicationsubscription', writeContent, true);         
    
}

function genericRequest(urlAdd, funcToCall, synchronous){
    /*
	 * based on api example from: 
	 * https://developer.mozilla.org/En/XMLHttpRequest/Using_XMLHttpRequest#Example.3a_Asynchronous_request
	 */
        
        
    if (funcToCall == "writeContent")
        funcToCall = writeContent;
    
    try {
        var req = new XMLHttpRequest();
    } catch(e) {
        alert('No support for XMLHTTPRequests');
        return;
    }
    var url = baseURL + urlAdd;
    
    var async = synchronous;
    req.open('GET', url, async);

    // add progress listener (differs from version to version...)
    req.onreadystatechange = function () {
        // state complete is of interest, only
        if (req.readyState == 4) {
            if (req.status == 200) {
                //call of the passed function (as parameter)
                funcToCall(req.responseText);
            //dump(req.responseText);
            } else {
                //if return code is another than 200 process error
                processRequestError(req.responseText);
                
            }
        }
    };

    // send request
    req.send();
        
}
