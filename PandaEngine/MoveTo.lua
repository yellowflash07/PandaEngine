

 
 --Mesh and sphere is the friendly Name  ,Value 1 is speed , Value2 is the name of the command
 

 vectorTable={0.0,0.0,0.0}

 locationTable={30.0,70.0,80.0}

 lightTable={0.0,0.0,0.0}

 curvePoint1={10.0,20.0,30.0}
 curvePoint2={20.0,20.0,40.0}
 curvePoint3={30.0,30.0,50.0}
 curvePoint4={30.0,50.0,80.0}
 curvePoint5={30.0,70.0,80.0}
 curvePoint6={30.0,90.0,80.0}

 
 curveTable={curvePoint1,curvePoint2,curvePoint3,curvePoint4,curvePoint5,curvePoint6}
 --curveTable={{1.0,2.0,3.0},2,3}

 moveTo = {"mesh","sphere",100,nil,"MoveTo","MeshFriendlyname"}
 orientTo = {"mesh","sphere",100,10, "OrientTo","MeshFriendlyname"}
 followObject = {"mesh","sphere",vectorTable,100, "FollowObject","MeshFriendlyname"}
 followCurve={"mesh",curveTable,nil,nil,"FollowCurve"}
 scaleUp= {"mesh",100.0,1.0,nil,"ScaleUp"}
 locationTrigger={"mesh",locationTable,0.0,scaleUp,"LocationTrigger"}
 lightControl={1,lightTable,100,nil,"LightControl"}
 

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


  
 