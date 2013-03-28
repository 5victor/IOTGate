package com.victor.iot;

import java.util.Vector;

public class Node {
	enum NodeType {ZC, ZR, ZED};
	NodeType type;
	String addr; /* as node identify */
	Vector<Endpoint> endpoints;
}
