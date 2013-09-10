
function genericRequestGet(urlAdd, funcToCall, synchronous){
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


function genericRequestPost(requestURL, requestContent,funcToCall, synchronous){
    /*
	 * based on api example from: 
	 * https://developer.mozilla.org/En/XMLHttpRequest/Using_XMLHttpRequest#Example.3a_Asynchronous_request
	 */
 
    try {
        var req = new XMLHttpRequest();
    } catch(e) {
        alert('No support for XMLHTTPRequests');
        return;
    }
    var url = baseURL + requestURL;
    
    var async = synchronous;
    req.open('POST', url, async);
    req.setRequestHeader('Content-Type', 'application/json');
    
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
    req.send(JSON.stringify(requestContent));
        
}


