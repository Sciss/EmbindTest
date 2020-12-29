addOnPostRun(function() {
  console.log("here: 1");
  var instance = new Module.MyClass(10, "hello");
  instance.incrementX();
  var x1 = instance.x; // 11
  console.log("x is now: " + x1);
  instance.x = 20; // 20
  var s = Module.MyClass.getStringFromInstance(instance); // "hello"
  console.log("string is now: " + s);
  instance.delete();
  console.log("here: 2");
});
