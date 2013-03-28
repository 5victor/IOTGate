package com.victor.iot;


public class Gateway {
	enum status {STOPPED, RUNNING, NODE_LEFT, NODE_ADDED};
	
	status status;
	
	//native method
	static {
		System.loadLibrary("IOTGateJNI");
	}
	
	public native void init();
	//Called by JNI when network change
	public void indicate()
	{
		//refresh TreeWindow so doesn't need IOTManage
	}
	
	//status modify by JNI?and use a function indacator
	public native status getStatus();
	public native int getNodeNum();
	
	// return a vector?
	public native Node getNode(int i);
	/* all in one Node
	//public native int getEndpointNum(Node node); //Endpoint
	public native Endpoint getEndpoint(Node node, int i);
	//public native int getClusterNum(Endpoint endpoint);
	public native Cluster getCluster(Endpoint endpoint, int i);
	*/
	
	//ZDO method
	
	//ZCL method
}
