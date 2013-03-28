package com.victor.types;

import java.util.Vector;

public class Tree<T> {
	T elem;
	Vector<Tree<T>> sons;
	
	public Tree(T e)
	{
		elem = e;
		sons = new Vector<Tree<T>>();
	}
}
