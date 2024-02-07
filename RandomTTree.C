//Exercise creating TTree
void RandomTTree() {

     TFile outputFile("random_TTree.root","recreate") ;
     TTree randomTree("Data", "TTree with Random Variables") ;
   
     int age;
     double height;
     bool sibling;

     randomTree.Branch("age", &age, "age/I");
     randomTree.Branch("height", &height, "height/D");
     randomTree.Branch("sibling", &sibling, "sibling/0");

     TRandom3 randomGenerator; 

     int amtPeople = 1000;

     for (int i = 0; i < amtPeople; i++) {

         age = randomGenerator.Integer(100);
         height = randomGenerator.Gaus(160,10);
         sibling = randomGenerator.Binomial(1, 0.5);

         randomTree.Fill();
     }

     randomTree.Write();

     outputFile.Close();

}

int main() {
    RandomTTree();
    return 0; 
}
