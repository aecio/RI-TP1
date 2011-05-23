#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WGroupBox>
#include <Wt/WTemplate>
#include <Wt/WAnchor>
#include <Wt/WGroupBox>
#include <Wt/WRadioButton>
#include <Wt/WButtonGroup>
#include <search/IndexSearcher.h>
#include <search/VSMIndexSearcher.h>
#include <search/BM25IndexSearcher.h>

using namespace Wt;

class WebSearcher : public WApplication {
public:

	WebSearcher(const WEnvironment& env);
	
private:
	enum IRModel { BM25 = 1, Vector = 2};
	
	IndexSearcher* searcher;
	WButtonGroup* bgModel;
	WLineEdit *edtQuery;
	WGroupBox *gbxResults;		

	void search();
	void btnVectorHandler();
	void btnBM25Handler();
};

WebSearcher::WebSearcher(const WEnvironment& env) : WApplication(env) {
	setTitle("Web Searcher");
	
	root()->setMargin(200, Left);
	root()->setMargin(200, Right);

	root()->addWidget(new WText("What are you looking for?<br/>"));

	edtQuery = new WLineEdit(root());
	edtQuery->setFocus();
	edtQuery->setTextSize(50);
	edtQuery->setMargin(15, Top);
	edtQuery->setMargin(15, Bottom);
	
	WPushButton *b = new WPushButton("Search", root());
	b->setMargin(10, Left);
	root()->addWidget(new WBreak());

	gbxResults = NULL;

	b->clicked().connect(this, &WebSearcher::search);
	edtQuery->enterPressed().connect(boost::bind(&WebSearcher::search, this));
	
	WGroupBox *container = new WGroupBox("Information Retrieval Model", root());

	// use a button group to logically group the 3 options
	WButtonGroup *bgModel = new WButtonGroup(this);

	WRadioButton *button;
	button = new WRadioButton("BM25", container);
	button->clicked().connect(this, &WebSearcher::btnBM25Handler);
	bgModel->addButton(button, BM25);

	button = new WRadioButton("Vector Space Model", container);
	button->clicked().connect(this, &WebSearcher::btnVectorHandler);
	bgModel->addButton(button, Vector);

	bgModel->setCheckedButton(bgModel->button(BM25));
	
	switch(bgModel->checkedId()){
		case Vector:
			searcher = new VSMIndexSearcher("indice");
			break;
		case BM25:
		default:
			searcher = new BM25IndexSearcher("indice");
	}
}

void WebSearcher::btnVectorHandler(){
	cout << "Changing model to: Vector Space Model." << endl;
	searcher = new VSMIndexSearcher("indice");
}

void WebSearcher::btnBM25Handler(){
	cout << "Changing model to: BM25." << endl;
	searcher = new BM25IndexSearcher("indice");
}

void WebSearcher::search() {
	if(gbxResults != NULL) {
		root()->removeWidget(gbxResults);
	}
	gbxResults = new WGroupBox("Search results for " + edtQuery->text() + ":");
	root()->addWidget(gbxResults);

	
	vector<Hit> hits = searcher->search(edtQuery->text().toUTF8 () , 10);
		
	if(hits.size() == 0){
		WText* score = new WText(gbxResults);
		score->setText("0 documents found.");
	} else {
		vector<Hit>::iterator it = hits.begin();
		for(; it != hits.end(); it++){
			WAnchor* link = new WAnchor(gbxResults);
			link->setText(it->doc.url);
			link->setRef(it->doc.url);
			new WBreak(gbxResults);
		
			WText* score = new WText(gbxResults);
			WString scoreStr("Score: {1}");
			scoreStr.arg(it->score);
			score->setText(scoreStr);
			
			new WBreak(gbxResults);
			new WBreak(gbxResults);		
		}
	}
	
	edtQuery->setFocus();
}

WApplication *createApplication(const WEnvironment& env) {
  return new WebSearcher(env);
}

int main(int argc, char **argv) {
  return WRun(argc, argv, &createApplication);
}
