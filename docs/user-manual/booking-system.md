The `BookStore` is an Object which managed Histograms for later universal access.

<style>
img[alt="dependency diagram"] {
	width: 450px;
}
</style>
See [dependency diagram](/diagrams/BookStore-Data.html) for further information.
[![dependency diagram](/diagrams/BookStore-Data.svg)](/diagrams/BookStore-Data.html)


## Entry Types

Depending on the use case, the BookStore can provide you're object in different ways.

* **Sinlge**

	> For usage in sequential code, minimal mangment overhead.
	
* **Multi**   
	 usage for may modifieing the object in parallel.
    * **MultiCopy**
		 instanciate object mutible times to avoid synchronisation.
		 Memory overhead.  
	 	 the number of instances must be passed to construct time.
	 	 
		 > **usecase:** high frequent writing operation.
		 
    * **MultiShared**
		cretaes object accassable with writer, to keep object valid locks are used.
		many synchronisation points.   
			
		 > **usecase:** much computing and only low frequent writing.	
		 	

## Creating new objects

1. call `book<Type>(path, name)` to set the type and position information for the Object.
2. call `single()`, `multiCopy(n)`, etc. for the type of usage you prefer.
3. pass the arguments for construction.

**examples**

* **for an single `RH1F`**
```cpp
	auto entry = store.book<RH1F>("/path/to/object/", "name").single()({"axis title", bins, min, max});
```
* **for an multiCopy, with 4 instances, `RH1F`**
```cpp
	auto entry = store.book<RH1F>("/path/", "name").multiCopy(4)({"a", bins, min, max});
```
* **for an single `RH2F`**
```cpp
	auto entry = store.book<RH2F>("/path/", "name").single()({"a", nA, minA, maxA}, {"b", nB, minB, maxB});
```

## Writing to an object

### creating an handle
`entry.handle()` will produce an new handle.  
For EntryMultiCopy you need to pass the id of the instance you want to access `entry.handle(id)`.

**example `RH1F`**
```cpp
	// sinlge
	Handle<RH1F> entrySingle.handle();

	// multy copy
	Handle<RH1F> entryMultiCopy.handle(0);

	//multy shared
	Handle<RH1F> entryMultiShared.handle();
```

### use the object specific modification functions.
*	Histogram
	 * `fill(position, weight)`  
	    add one datum to the histogram, position array based type which fileds equal to the number of dimensons from the histogram.    
	    e.g 1D `fill({x}, w)`, 2D `fill({x,y},w)`, etc.
	 * `fillN(span<…> positisns, span<…> weights)`  
	     add mutible dates to the histogram.

**example `RH1F`**
```cpp
	handle.fill({1}, 1);
	handle.fillN({{1}, {2}, {3}}, {1,1,1});
```


## reading final version of object

`entry.merged()` returns an const reference to the final object.

**example `RH1F`**
```cpp
	const RH1F& hist = entry.merged();
	// now you can reading the histogram normaly
	std::cout << hist.GetBinContent({0}) << '\n';
```


## Access created objects

### selection

Set of entries which full fill a condition. Can created from a BookStore
or a other Selection.

**features**
* iterate able  
* manually removing from elements
* access entries

### Condition

An Condition for filtering entries. Constructed with `ConditionBuilder`.

**atributes to filter**

* name: perfect match or regex
* path: perfect match or regex
* type: perfect match

**example**
```cpp
	Condition condition = ConditionBuilder()
		.setName("name") // perfect match to name required
		.setPath(std::regex("^p[^/]*h")); // path must match regex
```

**example** get a handle from every `RH1F` in '/path/to/dir'
```cpp

	Selection selection = store.find(
		ConditionBuilder().setPath("/path/to/dir/")
	);

	for(const entry& : selection) {
		auto oHandle = entry.handle<RH1F>();
		if(oHandle)
			handles<RH1F>.push_back(oHandle.value());
	}
```

