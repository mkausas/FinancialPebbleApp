
function getInfo(type) {
  // create http request
  var xmlHttp = null;
  xmlHttp = new XMLHttpRequest();
  
  var apiKey = "72aafb5e4adc36c1b33cbd9a7549853a";
  var requestURL = "";
  var requestType = "GET";
  switch (type) {
    // Find all bills for a specific account (GET); all billing information
    case 0:
      requestURL = "http://api.reimaginebanking.com/accounts/55cef43a2644c1aa1065164b/bills?key=" + apiKey;
      break;
      
    // Find all accounts (GET); contains balance 
    case 1:
      requestURL = "http://api.reimaginebanking.com/accounts?type=Credit%20Card&key=" + apiKey;  
      break;
  }
      
  xmlHttp.open( requestType, requestURL, false );
  xmlHttp.send( null );
  
  var obj = JSON.parse(xmlHttp.responseText);
  
  if (xmlHttp.status == 200) {
//     console.log("Got status of 200!");
  } else {
    console.log("Response not 200");
  }

  return obj;
}
  
// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");
  
//     var info = {};
    
    var type = 1;
    // Get capital one info
    var json = getInfo(type);
  
    console.log("Current Balance: " + json[0].balance);
//     info["balance"] = json[0].balance;
    var info = {"BALANCE" : json[0].balance};
    // grab the bill data
    json = getInfo(0);
    var numBills = json.length;
    console.log("Number of Bills: " + numBills);
    info["BILL_COUNT"] = numBills;
//     for (var i = 0; i < numBills; i++) {
//       var currentBill = json[i]; 
//       console.log("Bill " + i);
//       console.log("Payee: " + currentBill.payee);
//       console.log("Payment Date: " + currentBill.payment_date);
//       console.log("Payment Amount: " + currentBill.payment_amount);
//       info[i + ""] = { 
//         "payee": currentBill.payee,
//         "date": currentBill.payment_date,
//         "amount": currentBill.payment_amount
// //       }; 
//       info["1p"] = currentBill.payee;
//       info["1d"] = currentBill.payment_date;
//       info["1a"] = currentBill.payment_amount;
      
      
//       info[i + 1 + ""] = currentBill.payee;
//       info["1d"] = currentBill.payment_date;
//       info["1a"] = currentBill.payment_amount; 

//     }
    
    Pebble.sendAppMessage(info,
      function(e) {
        console.log('Successfully delivered message with transactionId=' + e.data.transactionId);
      },
      function(e) {
        console.log('Unable to deliver message with transactionId=' + e.data.transactionId + ' Error is: ' + e.error.message);
      }
    );
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    
    
  }                     
);