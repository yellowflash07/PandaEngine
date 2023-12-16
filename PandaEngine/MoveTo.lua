

 
 --Mesh and sphere is the friendly Name  ,Value 1 is speed , Value2 is the name of the command
 

 vectorTable={0.0,0.0,0.0}

 locationTable={0.0,0.0,0.0}

 lightTable={0.0,0.0,0.0}

 curvePoint1={0.0,0.0,0.0}
 curvePoint2={0.0,0.0,0.0}
 curvePoint3={0.0,0.0,0.0}

 
 curveTable={curvePoint1,curvePoint2,curvePoint3}

 moveTo = {"mesh","sphere",100,nil,"MoveTo","MeshFriendlyname"}
 orientTo = {"mesh","sphere",100,10, "OrientTo","MeshFriendlyname"}
 followObject = {"mesh","sphere",vectorTable,100, "FollowObject","MeshFriendlyname"}
 followCurve={"mesh",curveTable,nil,nil,"FollowCurve"}
 locationTrigger={"mesh",locationTable,0.0,nil,"LocationTrigger"}
 lightControl={1,lightTable,100,nil,"LightControl"}
 scaleUp={"mesh",100.0,1,nil,"ScaleUp"}

 commandGroups = 
 {
    moveTo,
    orientTo,
    followObject,
    followCurve,
    locationTrigger,
    lightControl,
    scaleUp
 }


  
 