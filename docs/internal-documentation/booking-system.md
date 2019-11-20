The `BookStore` is an Object which managed Histograms for later universal access.

<style>
img[alt="dependency diagram"] {
	width: 450px;
}
</style>
See [dependency diagram](/diagrams/BookStore-Data.html) for further information.
[![dependency diagram](/diagrams/BookStore-Data.svg)](/diagrams/BookStore-Data.html)


## Entry Types

Depending on the use case, the BookStore can provide your object in different ways.

* **Single**

	> For usage in sequential code, minimal management overhead.
	
* **Multi**   
	 usage for may modifying the object in parallel.
    * **MultiCopy**
		 instantiate object mutable times to avoid synchronisation.
		 Memory overhead.  
	 	 The number of instances must be passed to construct time.
	 	 
		 > **use case:** high frequent writing operation.
		 
    * **MultiShared**
		creates object accessible with writer, to keep object valid locks are used.
		Many synchronisation points.   
			
		 > **use case:** much computing and only low frequent writing.	
		 	

## Creating new objects

1. Construct `EntryData` (blueprint) for the object to book
2. Book object with `book(absolute-path, name, entry-data)`

**examples**

* **for a single `RH1F`**
```cpp
	EntryData entry-data = EntryData<RH1F>("title", {"axis title", bins, min, max});
	auto entry = store.book("/path/to/object/", "name", entry-data.single());
```
* **for a multi copy, with 4 instances, `RH1F`**
```cpp
	auto entry = store.book("/path/", "name", EntryData<RH1F>(axis).multiCopy(4));
```
* **for a single `RH2F`**
```cpp
	auto entry = store.book("/path/", "name", EntryData<RH2F>(axis1, axis2).single());
```

## Writing to an object

### creating a handle
`entry.handle()` will produce a new handle.  
For EntryMultiCopy you need to pass the id of the instance you want to access `entry.handle(id)`.

**example `RH1F`**
```cpp
	// single
	Handle<RH1F> entrySingle.handle();

	// multi copy
	Handle<RH1F> entryMultiCopy.handle(0);

	//multi shared
	Handle<RH1F> entryMultiShared.handle();
```

### use the object specific modification functions.
*	Histogram
	 * `fill(position, weight)`  
	    add one datum to the histogram, position array based type which fields equal to the number of dimensions from the histogram.    
	    e.g 1D `fill({x}, w)`, 2D `fill({x,y},w)`, etc.
	 * `fillN(span<…> positions, span<…> weights)`  
	     add mutable dates to the histogram.

**example `RH1F`**
```cpp
	handle.fill({1}, 1);
	handle.fillN({{1}, {2}, {3}}, {1,1,1});
```


## reading final version of object

`entry.merged()` returns a const reference to the final object.

**example `RH1F`**
```cpp
	const RH1F& hist = entry.merged();
	// now you can read the histogram normally
	std::cout << hist.GetBinContent({0}) << '\n';
```


## Access created objects

### selection

Set of entries which full fill a condition. Can created from a BookStore
or an other Selection.

**features**
* iterate able  
* manually removing from elements
* access entries

### Condition

A Condition for filtering entries. Constructed with `ConditionBuilder`.

**attributes to filter**

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

	try {
		for(const entry& : selection) {
			handles<RH1F>.push_back(entry.handle<RH1F>());
		}
	} catch (const marlin::BookStoreException&){}
```

