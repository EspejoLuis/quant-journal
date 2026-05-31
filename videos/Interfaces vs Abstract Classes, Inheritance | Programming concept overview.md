---
type: video
---
# Interfaces vs Abstract Classes, Inheritance | Programming concept overview

---

## 📝 Summary

[Youtube Video](https://youtu.be/5rs39FxBdcs?si=x-jDcD7tAcouCUF1) that describes the difference between abstract classes/methods and interface.

---

## 🔑 Key Points

- Common to most programming languages.
- Abstract Class
  - "**HAS-A**" relationship. Subclasses are somehow related.
  - The term "abstract" class means something not tangigble so it cannot be instantiated.
  - Example:

    ```c#
    // Superclass
    public abstract class Animal
    {
        public virtual void MakeSound()
        {
            Print("Generic Sound"); // may be implemented
        }

        public abstract void Move()
        {
            // must be implemented
        } 
    }
    // Subclass
    public class Cat: Animal
    {
        public override void MakeSound()
        {
            Print("Meow");
        }
        
        public override void Move()
        {
            // Do something
        }
    }
    ```

- Interface
  - "**IS-A**" relationship. Same functions need to be run on very different subclasses.
  - Interface cannot be instantiated.
  - Example:

    ```c#
    // Superclass
    public interface iAnimal
    {
        void MakeSound();
    }
    // Subclass
    public class Cat: iAnimal
    {
        void MakeSound()
        {
            Print("Meow")
        }
    }
    ```

---
